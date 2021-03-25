#include "GA_FireWeaponInstant.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_WaitTargetDataUsingActor.h"
#include "SquadLeader/AbilitySystem/Soldiers/Trace/SL_LineTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_FireWeaponInstant::UGA_FireWeaponInstant() : ServerWaitForClientTargetDataTask{ nullptr }, SourceWeapon{ nullptr }, TimeOfLastShoot{ -9999.f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::None;
	AbilityID = ESoldierAbilityInputID::None;
}

bool UGA_FireWeaponInstant::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	return SourceSoldier && Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
}

void UGA_FireWeaponInstant::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
	ASL_LineTrace* LineTrace = SourceWeapon->GetLineTraceTargetActor();

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !LineTrace)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	LineTrace->ResetSpread(); // Spread is only reseted here - Hence continuous fire won't reset

	ServerWaitForClientTargetDataTask = USL_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, false);
	ServerWaitForClientTargetDataTask->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	ServerWaitForClientTargetDataTask->ReadyForActivation();

	FireBullet();
}

void UGA_FireWeaponInstant::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ServerWaitForClientTargetDataTask)
		ServerWaitForClientTargetDataTask->EndTask();
}

void UGA_FireWeaponInstant::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_FireWeaponInstant::FireBullet()
{
	if (!SourceWeapon || FMath::Abs(UGameplayStatics::GetTimeSeconds(GetWorld())- TimeOfLastShoot) < SourceWeapon->GetTimeBetweenShots())
		return;

	bool bShouldProduceTargetDataOnServer = true;

	if (APlayerController* PC = CurrentActorInfo->PlayerController.Get(); PC)
	{
		if (!PC->IsLocalPlayerController())
			return;

		bShouldProduceTargetDataOnServer = false;
	}
	// else We assume the controller is an AIController

	// CHeckcost ability and cancel if necessqry

	FGameplayAbilityTargetingLocationInfo TraceStartLocation;
	TraceStartLocation.LiteralTransform = SourceWeapon->GetOwner()->GetActorTransform();

	ASL_LineTrace* LineTrace = SourceWeapon->GetLineTraceTargetActor();

	LineTrace->SetStartLocation(TraceStartLocation);
	LineTrace->TraceProfile = FCollisionProfileName{ SourceWeapon->CollisionProfileName };
	LineTrace->bIgnoreBlockingHits = false;
	LineTrace->SetShouldProduceTargetDataOnServer(bShouldProduceTargetDataOnServer);
	LineTrace->bUsePersistentHitResults = false;
	LineTrace->bTraceAffectsAimPitch = true;
	LineTrace->bUseAimingSpreadMod = false;
	LineTrace->MaxRange = SourceWeapon->GetMaxRange();
	LineTrace->BaseSpread = SourceWeapon->GetBaseSpread();
	LineTrace->TargetingSpreadIncrement = SourceWeapon->GetTargetingSpreadIncrement();
	LineTrace->TargetingSpreadMax = SourceWeapon->GetTargetingSpreadMax();

#if ENABLE_DRAW_DEBUG
	LineTrace->bDebug = SourceWeapon->bDebugTrace;
#endif

	USL_WaitTargetDataUsingActor* TaskWaitTarget = USL_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(this, NAME_None, EGameplayTargetingConfirmation::Instant, LineTrace, true);
	TaskWaitTarget->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	TaskWaitTarget->ReadyForActivation();

	TimeOfLastShoot = UGameplayStatics::GetTimeSeconds(GetWorld());

	UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, SourceWeapon->GetTimeBetweenShots());
	TaskWaitDelay->Activate();
	TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_FireWeaponInstant::FireBullet);
}

void UGA_FireWeaponInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& _Data)
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);

	// TODO: Remove one bullet

	if (UAnimMontage* FireMontage = SourceSoldier->WeaponFireMontage; FireMontage)
		SourceSoldier->PlayAnimMontage(FireMontage);

	// Apply is firing state to shooter - TODO: also add is fighting
	FGameplayEffectSpecHandle FiringEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_FiringStateClass, GetAbilityLevel());
	SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*FiringEffectSpecHandle.Data.Get());

	// Apply damages
	ApplyDamagesAndHits(_Data, SourceSoldier);
}

void UGA_FireWeaponInstant::ApplyDamagesAndHits(const FGameplayAbilityTargetDataHandle& _Data, ASoldier* _SourceSoldier)
{
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_DamageClass, GetAbilityLevel());
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), SourceWeapon->GetWeaponDamage());

	for (TWeakObjectPtr<AActor> Actor : _Data.Get(0)->GetActors())
	{
		if (ASoldier* TargetSoldier = Cast<ASoldier>(Actor); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
		{
			if (TargetSoldier->GetTeam() != _SourceSoldier->GetTeam())
				_SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetSoldier->GetAbilitySystemComponent());
		}
	}

	FGameplayEffectContextHandle EffectContext = DamageEffectSpecHandle.Data->GetEffectContext();
	EffectContext.AddHitResult(*_Data.Get(0)->GetHitResult());
}