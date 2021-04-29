#include "GameplayAbilitySoldier.h"
#include "AbilitySystemSoldier.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayerState.h"

UGameplayAbilitySoldier::UGameplayAbilitySoldier()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Movement")));
}

void UGameplayAbilitySoldier::ActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _OwnerInfo, const FGameplayAbilityActivationInfo _ActivationInfo, const FGameplayEventData* _TriggerEventData)
{
	if (ASoldierPlayer* SoldierPlayer = Cast<ASoldierPlayer>(_OwnerInfo->AvatarActor); SoldierPlayer && SoldierPlayer->IsLocallyControlled())
	{
		if (AbilityID == ESoldierAbilityInputID::Ability1 || AbilityID == ESoldierAbilityInputID::Ability2 || AbilityID == ESoldierAbilityInputID::Ability3)
		{
			SoldierPlayer->GetController<ASoldierPlayerController>()->NotifyMainAbilityCooldown(CooldownDuration.Value, AbilityID);
		}
		else if (GetClass() == SoldierPlayer->GetClassAbility1())
		{
			SoldierPlayer->GetController<ASoldierPlayerController>()->NotifyMainAbilityCooldown(CooldownDuration.Value, ESoldierAbilityInputID::Ability1);
		}
		else if (GetClass() == SoldierPlayer->GetClassAbility2())
		{
			SoldierPlayer->GetController<ASoldierPlayerController>()->NotifyMainAbilityCooldown(CooldownDuration.Value, ESoldierAbilityInputID::Ability2);
		}
		else if (GetClass() == SoldierPlayer->GetClassAbility3())
		{
			SoldierPlayer->GetController<ASoldierPlayerController>()->NotifyMainAbilityCooldown(CooldownDuration.Value, ESoldierAbilityInputID::Ability3);
		}
	}
	Super::ActivateAbility(_Handle, _OwnerInfo, _ActivationInfo, _TriggerEventData);
}

bool UGameplayAbilitySoldier::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle _InAbilityHandle, const bool _EndAbilityImmediately)
{
	UAbilitySystemSoldier* ASC = Cast<UAbilitySystemSoldier>(GetAbilitySystemComponentFromActorInfo());
	return ASC && ASC->BatchRPCTryActivateAbility(_InAbilityHandle, _EndAbilityImmediately);
}
void UGameplayAbilitySoldier::ExternalEndAbility()
{
	check(CurrentActorInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbilitySoldier::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}

const FGameplayTagContainer* UGameplayAbilitySoldier::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags(); ParentTags)
		MutableTags->AppendTags(*ParentTags);

	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UGameplayAbilitySoldier::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (UGameplayEffect* CooldownGE = GetCooldownGameplayEffect(); CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown")), CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}