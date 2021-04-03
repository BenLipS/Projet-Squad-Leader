#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "GA_SpawnPhysicalShield.generated.h"

UCLASS()
class SQUADLEADER_API UGA_SpawnPhysicalShield : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_SpawnPhysicalShield();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayTagContainer* _SourceTags = nullptr, const FGameplayTagContainer* _TargetTags = nullptr, OUT FGameplayTagContainer* _OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle _Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilityActivationInfo _ActivationInfo, const FGameplayEventData* _TriggerEventData) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class AShield> ShieldClass;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ShieldLifeSpan;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ShieldHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	float ShieldDistanceFromCaller;

	// Scale the shield mesh
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	FVector ShieldScale;

	void SpawnShield();
};