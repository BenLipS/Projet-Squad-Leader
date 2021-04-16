#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "GA_ReloadWeapon.generated.h"

UCLASS()
class SQUADLEADER_API UGA_ReloadWeapon : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_ReloadWeapon();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION()
	void ReloadWeapon();

	UFUNCTION()
	void MontageCompletedOrBlendedOut();

	UFUNCTION()
	void MontageInterruptedOrCancelled();

	// Cache values
	ASL_Weapon* SourceWeapon = nullptr;
};
