#include "GA_LaunchGrenade.h"
#include "../../../Soldiers/Soldier.h"

UGA_LaunchGrenade::UGA_LaunchGrenade()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::None; // Must be defined in the BP subclasses
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillGrenadeTag); // Must define another tag in the BP subclasses to be more specific
}

void UGA_LaunchGrenade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		// Create grenade then launch it
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = ActorInfo->AvatarActor.Get();
		SpawnInfo.Instigator = ActorInfo->AvatarActor.Get()->GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		GetWorld()->SpawnActor<ASL_Projectile>(MyProjectile, SpawnInfo.Owner->GetActorLocation(), Cast<ASoldier>(SpawnInfo.Owner)->CurrentCameraComponent->GetForwardVector().Rotation(), SpawnInfo);

		//GEngine->AddOnScreenDebugMessage(7891, 0.5f, FColor::Green, FString::Printf(TEXT("Grenade launched")));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

bool UGA_LaunchGrenade::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_LaunchGrenade::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_LaunchGrenade::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_LaunchGrenade::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}