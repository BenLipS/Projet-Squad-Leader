#include "GA_GiveOrder.h"
#include "../../../Soldiers/Players/SoldierPlayer.h"
#include "SquadLeader/AI/AISquadManager.h"

UGA_GiveOrder::UGA_GiveOrder()
{
	AbilityInputID = ESoldierAbilityInputID::GiveOrder;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(ASoldier::SkillGiveOrderTag);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_GiveOrder::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		if (ASoldierPlayer* Soldier = Cast<ASoldierPlayer>(ActorInfo->AvatarActor.Get()); Soldier)
		{
			//Activation();
			// Init task
			UAbilityTask_WaitTargetData *Task = UAbilityTask_WaitTargetData::WaitTargetData(this, FName("WaitTargetData Give Order"), TEnumAsByte<EGameplayTargetingConfirmation::Type>(EGameplayTargetingConfirmation::UserConfirmed), TargetActorClass);

			AGameplayAbilityTargetActor* SpawnedActor;

			Task->ReadyForActivation();

			if (!Task->BeginSpawningActor(this, TargetActorClass, SpawnedActor))
				return;

			Task->FinishSpawningActor(this, SpawnedActor);

			// Bind end task
			Task->ValidData.AddDynamic(this, &UGA_GiveOrder::OnOrderValid);
			Task->Cancelled.AddDynamic(this, &UGA_GiveOrder::OnOrderCancelled);

			// Target actor position
			FGameplayAbilityTargetingLocationInfo TargetingLocationInfo = MakeTargetLocationInfoFromOwnerActor();
			TargetingLocationInfo.LiteralTransform.SetLocation(Soldier->GetActorForwardVector() * 200 + TargetingLocationInfo.LiteralTransform.GetLocation());
			SpawnedActor->StartLocation = TargetingLocationInfo;
			
			// Apply Gameplay effect
			FGameplayEffectSpecHandle GivingOrderEffectSpecHandle = MakeOutgoingGameplayEffectSpec(GivingOrderGameplayEffect, GetAbilityLevel());
			Soldier->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*GivingOrderEffectSpecHandle.Data.Get());
		}
	}
}

bool UGA_GiveOrder::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GiveOrder::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	//	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGA_GiveOrder::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_GiveOrder::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ASoldier* Soldier = Cast<ASoldier>(ActorInfo->AvatarActor.Get()); Soldier)
	{
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(ASoldier::StateGivingOrderTag);
		Soldier->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(EffectTagsToRemove);
	}
}

void UGA_GiveOrder::OnOrderValid(const FGameplayAbilityTargetDataHandle& _Data)
{
	if (ASoldierPlayer* Soldier = Cast<ASoldierPlayer>(CurrentActorInfo->AvatarActor.Get()); Soldier)
	{
		if (Soldier->GetLocalRole() == ROLE_Authority)
		{
			FVector_NetQuantize Location = _Data.Data[0]->GetHitResult()->Location;
			Soldier->GetSquadManager()->UpdateMission(MissionType::Attack, FVector{ Location.X, Location.Y, Location.Z });
		}
	}
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_GiveOrder::OnOrderCancelled(const FGameplayAbilityTargetDataHandle& _Data)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}