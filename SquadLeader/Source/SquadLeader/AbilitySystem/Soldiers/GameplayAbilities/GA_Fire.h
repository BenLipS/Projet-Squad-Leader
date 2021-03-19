#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "../GameplayEffects/States/GE_StateFiring.h"
#include "GA_Fire.generated.h"

UCLASS()
class SQUADLEADER_API UGA_Fire : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_Fire();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGE_StateFiring> FiringGameplayEffect;
};