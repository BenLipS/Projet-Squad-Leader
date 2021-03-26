// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_WallVision.h"

#include "../../../Soldiers/Soldier.h"
#include "../../../Soldiers/Players/SoldierPlayerController.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"


UGA_WallVision::UGA_WallVision()
{
	AbilityInputID = ESoldierAbilityInputID::WallVision;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.WallVision")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_WallVision::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
	{
		if (ASoldierPlayerController* PC = Soldier->GetController< ASoldierPlayerController>(); PC)
		{
			return true;
		}
	}

	return false;
}

void UGA_WallVision::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
		{
			if (ASoldierPlayerController* PC = Soldier->GetController< ASoldierPlayerController>(); PC)
			{
				PC->OnWallVisionActivate();
				UAbilityTask_WaitDelay* TaskWaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, 2.f);
				TaskWaitDelay->Activate();
				TaskWaitDelay->OnFinish.AddDynamic(this, &UGA_WallVision::EndWallVision);
			}
		}
	}
}

void UGA_WallVision::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_WallVision::EndWallVision()
{
	if (ASoldier* Soldier = Cast<ASoldier>(CurrentActorInfo->AvatarActor.Get()); Soldier)
	{
		if (ASoldierPlayerController* PC = Soldier->GetController< ASoldierPlayerController>(); PC)
		{
			PC->OnWallVisionDeactivate();
		}
	}	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
