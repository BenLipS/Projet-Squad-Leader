#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "GA_QuickDash.generated.h"

UCLASS()
class SQUADLEADER_API UGA_QuickDash : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_QuickDash();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float Strength = 2000.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float Duration = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	bool bIsAdditive = false;

protected:
	UFUNCTION()
	void OnDashEnded();
};
