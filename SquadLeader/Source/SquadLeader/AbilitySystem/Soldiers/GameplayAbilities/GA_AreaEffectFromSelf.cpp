#include "GA_AreaEffectFromSelf.h"
#include "../../../Soldiers/Soldier.h"

UGA_AreaEffectFromSelf::UGA_AreaEffectFromSelf()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AreaEffectFromSelf"))); // Must define another tag in the BP subclasses to be more specific
}

void UGA_AreaEffectFromSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		APawn* AreaEffectOwner = Cast<APawn>(ActorInfo->AvatarActor.Get());
		AAreaEffect* AreaEffect = GetWorld()->SpawnActorDeferred<AAreaEffect>(AreaEffectClass,FTransform{ AreaEffectOwner->GetActorRotation(), AreaEffectOwner->GetActorLocation() }, AreaEffectOwner, AreaEffectOwner, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (AreaEffect)
		{
#ifdef UE_BUILD_DEBUG
			AreaEffect->bDebugTrace = bDebugTraceAreaEffect;
#endif
			AreaEffect->FinishSpawning(FTransform{ AreaEffectOwner->GetActorRotation(), AreaEffectOwner->GetActorLocation() });

			//Spawn Effect
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AreaFX = GetWorld()->SpawnActor<AActor>(AreaFXClass, Cast<ASoldier>(ActorInfo->AvatarActor)->GetLocation(), { 0,0,0 }, SpawnInfo);
			if (DoesEffectFollowActor)AreaFX->AttachToActor(Cast<ASoldier>(ActorInfo->AvatarActor), FAttachmentTransformRules::KeepWorldTransform);
		}
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UGA_AreaEffectFromSelf::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_AreaEffectFromSelf::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_AreaEffectFromSelf::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_AreaEffectFromSelf::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}