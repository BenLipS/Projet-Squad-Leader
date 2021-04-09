#include "GA_GiveOrder.h"
#include "../../../Soldiers/Players/SoldierPlayer.h"
#include "../../../Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/AI/AISquadManager.h"

UGA_GiveOrder::UGA_GiveOrder()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = ESoldierAbilityInputID::GiveOrder;
	AbilityID = ESoldierAbilityInputID::None;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.GiveOrder")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.GivingOrder")));
}

void UGA_GiveOrder::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

		/*if (ASoldierPlayer* Soldier = Cast<ASoldierPlayer>(ActorInfo->AvatarActor.Get()); Soldier)
		{
			if (Soldier->GetLocalRole() == ROLE_Authority)
			{
				if (Soldier->GetSquadManager()->GetMission()->Type != MissionType::Formation)
				{

					Soldier->GetSquadManager()->UpdateMission(MissionType::Formation, FVector{ 0, 0, 0 });

					CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
					return;
				}
			}

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
		}*/
		//Call affichage
	}
}

bool UGA_GiveOrder::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GiveOrder::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
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