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

UGA_FireWeaponInstant::UGA_FireWeaponInstant() :
ServerWaitForClientTargetDataTask{ nullptr },
SourceWeapon{ nullptr },
SourceSoldier { nullptr },
TimeOfLastShoot{ -9999.f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon")));
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
	if (APlayerController* PC = CurrentActorInfo->PlayerController.Get(); PC)
	{
		if (!PC->IsLocalPlayerController())
			return;
	}

	// Too soon to shoot or is reloading
	const constexpr float epsilon = 0.01; // Error tolerance

	if (FMath::Abs(UGameplayStatics::GetTimeSeconds(GetWorld()) - TimeOfLastShoot) - epsilon < SourceWeapon->GetTimeBetweenShots()
		|| SourceSoldier->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon"))))
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
		ReloadWeapon();
		return;
	}

	// Update line tracing
	FGameplayAbilityTargetingLocationInfo TraceStartLocation;
	TraceStartLocation.LiteralTransform = SourceWeapon->GetOwner()->GetActorTransform();

	LineTrace->SetStartLocation(TraceStartLocation);

	// Camera Shake
	SourceSoldier->ShakeCamera(SourceSoldier->GetCameraShakeFireClass());

	// Wait target data
	USL_WaitTargetDataUsingActor* TaskWaitTarget = USL_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(this, NAME_None, EGameplayTargetingConfirmation::Instant, LineTrace, true);
	TaskWaitTarget->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	TaskWaitTarget->ReadyForActivation();

	// Wait for the next fire
	TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
	TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_FireWeaponInstant::FireBullet);
	TaskWaitDelay->ReadyForActivation();

	TimeOfLastShoot = UGameplayStatics::GetTimeSeconds(GetWorld());
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

	// Gameplay cue
	UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(GetAbilitySystemComponentFromActorInfo());

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddHitResult(*Data->GetHitResult());

	FGameplayCueParameters GC_Parameters;
	GC_Parameters.Instigator = SourceSoldier;
	GC_Parameters.SourceObject = SourceWeapon;
	GC_Parameters.EffectContext = EffectContext;
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
	if (SourceSoldier->GetTeam() != _TargetSoldier->GetTeam())
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