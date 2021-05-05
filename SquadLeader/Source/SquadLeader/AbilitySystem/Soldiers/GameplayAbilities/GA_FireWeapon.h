#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "GA_FireWeapon.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API UGA_FireWeapon : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_FireWeapon();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void HandleFire();
	void ReloadWeapon();

protected:
	ASL_Weapon* SourceWeapon;
	ASoldier* SourceSoldier;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<class UGA_FireWeaponInstant> GA_FireWeaponInstantClass;

	class UGA_FireWeaponInstant* GA_FireWeaponInstantInstance;

	FGameplayAbilitySpecHandle InstantAbilityHandle;
};