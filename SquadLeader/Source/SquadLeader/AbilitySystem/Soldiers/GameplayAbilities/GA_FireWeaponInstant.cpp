#include "GA_FireWeaponInstant.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "SquadLeader/Weapons/Shield.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "../../../Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/AbilitySystem/Soldiers/Trace/SL_LineTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_ServerWaitForClientTargetData.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_WaitTargetDataUsingActor.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"

UGA_FireWeaponInstant::UGA_FireWeaponInstant() :
ServerWaitForClientTargetDataTask{ nullptr },
SourceWeapon{ nullptr },
SourceSoldier { nullptr },
TimeOfLastShoot{ -9999.f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Reloading")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell")));
}

bool UGA_FireWeaponInstant::CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags, const FGameplayTagContainer* _TargetTags, OUT FGameplayTagContainer* _OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(_ActorInfo->AvatarActor);
	return Soldier && Cast<ASL_Weapon>(Soldier->GetCurrentWeapon()) && Super::CanActivateAbility(_Handle, _ActorInfo, _SourceTags, _TargetTags, _OptionalRelevantTags);
}

void UGA_FireWeaponInstant::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	ConfigLineTrace();

	ServerWaitForClientTargetDataTask = USL_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, false);
	ServerWaitForClientTargetDataTask->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	ServerWaitForClientTargetDataTask->ReadyForActivation();

	FireBullet();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_FireWeaponInstant::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ServerWaitForClientTargetDataTask && ServerWaitForClientTargetDataTask->IsActive())
		ServerWaitForClientTargetDataTask->EndTask();

	if (TaskWaitDelay && TaskWaitDelay->IsActive())
		TaskWaitDelay->EndTask();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FireWeaponInstant::FireBullet()
{
	APlayerController* PC = CurrentActorInfo->PlayerController.Get();
	const bool bIsLocal = !IsValid(PC) || PC->IsLocalPlayerController();

	// Too soon to shoot or is reloading
	const constexpr float epsilon = 0.01; // Error tolerance

	if (FMath::Abs(UGameplayStatics::GetTimeSeconds(GetWorld()) - TimeOfLastShoot) + epsilon < SourceWeapon->GetTimeBetweenShots()
		|| SourceSoldier->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Weapon.Reloading"))))
	{
		// Wait for the next fire
		TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
		TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_FireWeaponInstant::FireBullet);
		TaskWaitDelay->ReadyForActivation();
		return;
	}

	// Need to reload
	if (!SourceWeapon->HasAmmo() && !SourceWeapon->HasInfiniteAmmo())
	{
		UAkGameplayStatics::PostEventByName("Rifle_Out_of_ammo", SourceWeapon);//maybe play cliking sound here
		if (bIsLocal)
			ReloadWeapon();
		return;
	}

	// Update line tracing
	FGameplayAbilityTargetingLocationInfo TraceStartLocation;
	TraceStartLocation.LiteralTransform = FTransform{ SourceWeapon->GetWeaponMesh()->GetSocketLocation(SourceWeapon->GetMuzzleAttachPoint()) };

	LineTrace->SetStartLocation(TraceStartLocation);

	FireAnimation(PredictFireHitResult());

	// Wait for the next fire
	TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
	TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_FireWeaponInstant::FireBullet);
	TaskWaitDelay->ReadyForActivation();

	TimeOfLastShoot = UGameplayStatics::GetTimeSeconds(GetWorld());

	if (!bIsLocal)
		return;

	// Wait target data
	USL_WaitTargetDataUsingActor* TaskWaitTarget = USL_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(this, NAME_None, EGameplayTargetingConfirmation::Instant, LineTrace, true);
	TaskWaitTarget->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	TaskWaitTarget->ReadyForActivation();
}

void UGA_FireWeaponInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& _Data)
{
	// We only work on the last data element which is the blocked element - TODO: Check rare case where it is not a blocked element. It is possible if we fire through an ally shield but don't reach anything else
	if (_Data.Data.Num() <= 0 || !_Data.IsValid(_Data.Data.Num() - 1))
		return;

	if (UAnimMontage* FireMontage = SourceWeapon->FireMontage; FireMontage)
		SourceSoldier->PlayAnimMontage(FireMontage);

	ApplyEffectsToSource();

	const FGameplayAbilityTargetData* Data = _Data.Get(_Data.Data.Num() - 1);
	const float BaseWeaponDamage = SourceWeapon->GetWeaponDamage();

	for (TWeakObjectPtr<AActor> Actor : Data->GetActors())
	{
		// Target is soldier
		if (ASoldier* TargetSoldier = Cast<ASoldier>(Actor); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
		{
			if (ApplyDamages(_Data, TargetSoldier, BaseWeaponDamage))
				TargetSoldier->OnReceiveDamage(Data->GetHitResult()->ImpactPoint, Data->GetHitResult()->TraceStart);
		}
		// Target is shield
		else if (AShield* Shield = Cast<AShield>(Actor); Shield)
		{
			ApplyDamages(Shield, BaseWeaponDamage);
		}
	}
}

void UGA_FireWeaponInstant::FireAnimation(const FHitResult& _HitResult)
{
	// Camera Shake
	SourceSoldier->ShakeCamera(SourceSoldier->GetCameraShakeFireClass());

	// Gameplay cue with predicted fire
	UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(GetAbilitySystemComponentFromActorInfo());

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddHitResult(_HitResult);

	FGameplayCueParameters GC_Parameters;
	GC_Parameters.Instigator = SourceSoldier;
	GC_Parameters.SourceObject = SourceWeapon;
	GC_Parameters.EffectContext = EffectContext;

	//DoAnimation(GC_Parameters);
	SourceSoldier->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.FireWeapon.Instant")), GC_Parameters);
}

void UGA_FireWeaponInstant::ApplyEffectsToSource()
{
	SourceWeapon->DecrementAmmo();

	FGameplayEffectSpecHandle FiringEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_FiringStateClass, GetAbilityLevel());
	SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*FiringEffectSpecHandle.Data.Get());
}

bool UGA_FireWeaponInstant::ApplyDamages(const FGameplayAbilityTargetDataHandle& _Data, ASoldier* _TargetSoldier, const float _Damage)
{
	if (SourceSoldier->GetTeam() != _TargetSoldier->GetTeam() && _TargetSoldier->IsAlive())
	{
		float FinalDamage = _Damage;
		bool bIsHeadShot = false;

		if (IsHeadShot(*_Data.Data[0].Get()->GetHitResult()))
		{
			FinalDamage *= SourceWeapon->GetHeadShotMultiplier();
			bIsHeadShot = true;
		}

		// Notify HUD for hit marker
		if (ASoldierPlayerController* PC = SourceSoldier->GetController<ASoldierPlayerController>(); PC)
			PC->NotifySoldierHit(FinalDamage, bIsHeadShot);

		// Apply damages
		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_DamageClass, GetAbilityLevel());
		DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), FinalDamage);

		UAbilitySystemSoldier* TargetASC = _TargetSoldier->GetAbilitySystemComponent();
		SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetASC);

		return true;
	}
	return false;
}

bool UGA_FireWeaponInstant::ApplyDamages(AShield* _Shield, const float _Damages)
{
	if (_Shield->GetTeam() != SourceSoldier->GetTeam())
	{
		_Shield->ApplyDamages(_Damages);
		return true;
	}
	return false;
}

bool UGA_FireWeaponInstant::IsHeadShot(const FHitResult& _HitResult) const
{
	return _HitResult.BoneName.IsEqual(FName{ "head" }, ENameCase::IgnoreCase);
}

void UGA_FireWeaponInstant::ReloadWeapon()
{
	SourceSoldier->ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.ReloadWeapon")));
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_FireWeaponInstant::ConfigLineTrace()
{
	if (LineTrace = SourceWeapon->GetLineTraceTargetActor(); !LineTrace)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	LineTrace->SetInstigator(SourceSoldier);
	LineTrace->ResetSpread(); // Spread is only reseted here - Continuous fire won't reset then
	LineTrace->TraceProfile = FCollisionProfileName{ SourceWeapon->CollisionProfileName };
	LineTrace->bIgnoreBlockingHits = false;
	LineTrace->bUsePersistentHitResults = false;
	LineTrace->MaxRange = SourceWeapon->GetMaxRange();
	LineTrace->BaseSpread = SourceWeapon->GetBaseSpread();
	LineTrace->AimingSpreadMod = SourceWeapon->GetAimingSpreadMod();
	LineTrace->TargetingSpreadIncrement = SourceWeapon->GetTargetingSpreadIncrement();
	LineTrace->TargetingSpreadMax = SourceWeapon->GetTargetingSpreadMax();
	LineTrace->SetShouldProduceTargetDataOnServer(!CurrentActorInfo->PlayerController.Get()); // Produce Target Data On Server only if the controller is an AI

#if UE_BUILD_DEBUG
	LineTrace->bDebug = SourceWeapon->bDebugTrace;
#endif
}

FHitResult UGA_FireWeaponInstant::PredictFireHitResult()
{
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ASL_LineTrace), false);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(SourceSoldier);
	Params.bIgnoreBlocks = false;

	TArray<FHitResult> OutHitResults;

	const FVector Start = LineTrace->GetStartLocation().GetTargetingTransform().GetLocation();
	const FVector End = SourceSoldier->GetLookingAtPosition();

	GetWorld()->LineTraceMultiByProfile(OutHitResults, Start, End, LineTrace->TraceProfile.Name, Params);
	FilterTraceWithShield(OutHitResults);

	if (OutHitResults.Num() <= 0)
	{
		// If there were no hits, add a default HitResult at the end of the trace
		FHitResult HitResult;
		HitResult.TraceStart = Start;
		HitResult.TraceEnd = End;
		HitResult.Location = End;
		HitResult.ImpactPoint = End;
		OutHitResults.Add(HitResult);
	}
	return OutHitResults.Last();
}

void UGA_FireWeaponInstant::FilterTraceWithShield(TArray<FHitResult>& _HitResults)
{
	for (int32 i = 0; i < _HitResults.Num(); ++i)
	{
		if (AShield* Shield = Cast<AShield>(_HitResults[i].Actor); Shield)
		{
			ASoldier* OwnerOfShield = Cast<ASoldier>(Shield->GetInstigator());

			// Remove every data after the shield since it should block the trace
			if (SourceSoldier && OwnerOfShield && SourceSoldier->GetTeam() != OwnerOfShield->GetTeam())
			{
				_HitResults.RemoveAt(FMath::Min(i + 1, _HitResults.Num() - 1), _HitResults.Num() - i - 1);
				return;
			}
		}
	}
}