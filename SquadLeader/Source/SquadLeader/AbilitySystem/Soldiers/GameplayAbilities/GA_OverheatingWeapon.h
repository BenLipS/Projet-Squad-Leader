#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "GA_OverheatingWeapon.generated.h"

UCLASS()
class SQUADLEADER_API UGA_OverheatingWeapon : public UGameplayAbilitySoldier
{
	GENERATED_BODY()
	
public:
	UGA_OverheatingWeapon();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* _ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	ASL_Weapon* SourceWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float TimeOverHeat;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Stats")
	float TimeBetweenShootMultiplier;

	UFUNCTION()
	void EndOverHeat();
//Effect
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Particles")
	TSubclassOf<AActor> OverHeatFXClass;

	UPROPERTY()
	AActor* OverHeatFX;
};
