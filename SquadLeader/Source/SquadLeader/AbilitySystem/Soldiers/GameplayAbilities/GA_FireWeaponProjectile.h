#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "../../../Projectile/SL_Projectile.h"
#include "GA_FireWeaponProjectile.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API UGA_FireWeaponProjectile : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_FireWeaponProjectile();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	ASL_Weapon* SourceWeapon;
	ASoldier* SourceSoldier;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASL_Projectile> ProjectileClass;
};