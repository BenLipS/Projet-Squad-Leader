#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
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
};
