#include "GA_FireWeaponInstant.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_WaitTargetDataUsingActor.h"
#include "SquadLeader/AbilitySystem/Soldiers/Trace/SL_LineTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "SquadLeader/SL_BlueprintFunctionLibrary.h" // TODO Should we develop an utility librairy or do these functions in ASC ?

UGA_FireWeaponInstant::UGA_FireWeaponInstant() : ServerWaitForClientTargetDataTask { nullptr }, SourceWeapon { nullptr }, TimeOfLastShoot { -9999.f }
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::None;
	AbilityID = ESoldierAbilityInputID::None;
}

bool UGA_FireWeaponInstant::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// TODO: Has enough bullet

	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	return SourceSoldier && Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());
}

void UGA_FireWeaponInstant::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ASL_LineTrace* LineTrace = SourceWeapon->GetLineTraceTargetActor();

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !LineTrace)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASoldier* SourceSoldier = Cast<ASoldier>(ActorInfo->AvatarActor);
	SourceWeapon = Cast<ASL_Weapon>(SourceSoldier->GetCurrentWeapon());

	LineTrace->ResetSpread(); // Spread is only reseted here - Hence continuous fire won't reset

	ServerWaitForClientTargetDataTask = USL_ServerWaitForClientTargetData::ServerWaitForClientTargetData(this, NAME_None, false);
	ServerWaitForClientTargetDataTask->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	ServerWaitForClientTargetDataTask->ReadyForActivation();

	FireBullet();
}

void UGA_FireWeaponInstant::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
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
	LineTrace->TraceProfile = FCollisionProfileName{ FName{"Soldier"} };
	LineTrace->bIgnoreBlockingHits = false;
	LineTrace->SetShouldProduceTargetDataOnServer(bShouldProduceTargetDataOnServer);
	LineTrace->bUsePersistentHitResults = false;
	LineTrace->bDebug = true; // TODO: use endif
	LineTrace->bTraceAffectsAimPitch = true;
	LineTrace->bUseAimingSpreadMod = false;
	LineTrace->MaxRange = 999'999.f;
	
	USL_WaitTargetDataUsingActor* Task = USL_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(this, NAME_None, EGameplayTargetingConfirmation::Instant, LineTrace, true);
	Task->ValidData.AddDynamic(this, &UGA_FireWeaponInstant::HandleTargetData);
	Task->ReadyForActivation();

	TimeOfLastShoot = UGameplayStatics::GetTimeSeconds(GetWorld());
}

void UGA_FireWeaponInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& _Data)
{
	ASoldier* SourceSoldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor);

	// TODO: Remove one bullet

	if (UAnimMontage* FireMontage = SourceSoldier->WeaponFireMontage; FireMontage)
		SourceSoldier->PlayAnimMontage(FireMontage);

	// Apply is firing state to shooter - TODO: also add is fighting
	FGameplayEffectSpecHandle FiringEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_FiringState, GetAbilityLevel());
	SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*FiringEffectSpecHandle.Data.Get());

	// Apply damages
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GE_Damage, GetAbilityLevel());
	DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), SourceWeapon->GetWeaponDamage());

	for (TWeakObjectPtr<AActor> Actor : _Data.Get(0)->GetActors())
	{
		if (ASoldier* TargetSoldier = Cast<ASoldier>(Actor); TargetSoldier && TargetSoldier->GetAbilitySystemComponent())
			SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetSoldier->GetAbilitySystemComponent());
			//SourceSoldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
	}

	FGameplayEffectContextHandle EffectContext = DamageEffectSpecHandle.Data->GetEffectContext();
	EffectContext.AddHitResult(*_Data.Get(0)->GetHitResult());
}