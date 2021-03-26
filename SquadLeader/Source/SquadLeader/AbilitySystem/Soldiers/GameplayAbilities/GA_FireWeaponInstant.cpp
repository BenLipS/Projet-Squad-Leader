#include "GA_FireWeaponInstant.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_WaitTargetDataUsingActor.h"
#include "SquadLeader/AbilitySystem/Soldiers/Trace/SL_LineTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_FireWeaponInstant::UGA_FireWeaponInstant() :
ServerWaitForClientTargetDataTask{ nullptr },
SourceWeapon{ nullptr },
SourceSoldier { nullptr },
TimeOfLastShoot{ -9999.f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::None;
	AbilityID = ESoldierAbilityInputID::None;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon")));
}

bool UGA_FireWeaponInstant::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	return Soldier && Cast<ASL_Weapon>(Soldier->GetCurrentWeapon());
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
}

void UGA_FireWeaponInstant::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ServerWaitForClientTargetDataTask && ServerWaitForClientTargetDataTask->IsActive())
		ServerWaitForClientTargetDataTask->EndTask();
}

void UGA_FireWeaponInstant::FireBullet()
{
	if (APlayerController* PC = CurrentActorInfo->PlayerController.Get(); PC)
	{
		if (!PC->IsLocalPlayerController())
			return;
	}

	// Too soon to shoot or is reloading
	if (FMath::Abs(UGameplayStatics::GetTimeSeconds(GetWorld())- TimeOfLastShoot) < SourceWeapon->GetTimeBetweenShots()
		|| SourceSoldier->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.ReloadingWeapon"))))
		return;

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

	// Wait target data
	USL_WaitTargetDataUsingActor* TaskWaitTarget = USL_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(this, NAME_None, EGameplayTargetingConfirmation::Instant, LineTrace, true);
	TaskWaitTarget->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	TaskWaitTarget->ReadyForActivation();

	// Wait for the next fire
	UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
	TaskWaitDelay->Activate();
	TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_FireWeaponInstant::FireBullet);

	TimeOfLastShoot = UGameplayStatics::GetTimeSeconds(GetWorld());
}

void UGA_FireWeaponInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& _Data)
{
	if (!_Data.IsValid(0))
		return;

	if (UAnimMontage* FireMontage = SourceSoldier->WeaponFireMontage; FireMontage)
		SourceSoldier->PlayAnimMontage(FireMontage);

	ApplyEffectsToSource();

	for (TWeakObjectPtr<AActor> Actor : _Data.Get(0)->GetActors())
	{
		if (ASoldier* TargetSoldier = Cast<ASoldier>(Actor); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
			ApplyDamages(_Data, TargetSoldier->GetAbilitySystemComponent());
	}
	
	FGameplayEffectSpecHandle FiringEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_FiringStateClass, GetAbilityLevel());
	SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*FiringEffectSpecHandle.Data.Get());
}

void UGA_FireWeaponInstant::ApplyEffectsToSource()
{
	SourceWeapon->DecrementAmmo();
}

void UGA_FireWeaponInstant::ApplyDamages(const FGameplayAbilityTargetDataHandle& _Data, UAbilitySystemComponent* _TargetASC)
{
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_DamageClass, GetAbilityLevel());
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), SourceWeapon->GetWeaponDamage());

	if (Cast<ASoldier>(_TargetASC->GetAvatarActor())->GetTeam() != SourceSoldier->GetTeam())
		SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), _TargetASC);

	FGameplayEffectContextHandle EffectContext = DamageEffectSpecHandle.Data->GetEffectContext();
	EffectContext.AddHitResult(*_Data.Get(0)->GetHitResult());
}

void UGA_FireWeaponInstant::ReloadWeapon()
{
	SourceSoldier->ActivateAbility(ASoldier::SkillReloadWeaponTag);
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_FireWeaponInstant::ConfigLineTrace()
{
	if (LineTrace = SourceWeapon->GetLineTraceTargetActor(); !LineTrace)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	LineTrace->ResetSpread(); // Spread is only reseted here - Continuous fire won't reset then
	LineTrace->TraceProfile = FCollisionProfileName{ SourceWeapon->CollisionProfileName };
	LineTrace->bIgnoreBlockingHits = false;
	LineTrace->bUsePersistentHitResults = false;
	LineTrace->bTraceAffectsAimPitch = true;
	LineTrace->bUseAimingSpreadMod = false;
	LineTrace->MaxRange = SourceWeapon->GetMaxRange();
	LineTrace->BaseSpread = SourceWeapon->GetBaseSpread();
	LineTrace->TargetingSpreadIncrement = SourceWeapon->GetTargetingSpreadIncrement();
	LineTrace->TargetingSpreadMax = SourceWeapon->GetTargetingSpreadMax();
	LineTrace->SetShouldProduceTargetDataOnServer(!CurrentActorInfo->PlayerController.Get()); // Produce Target Data On Server only if the controller is an AI

#if ENABLE_DRAW_DEBUG
	LineTrace->bDebug = SourceWeapon->bDebugTrace;
#endif
}