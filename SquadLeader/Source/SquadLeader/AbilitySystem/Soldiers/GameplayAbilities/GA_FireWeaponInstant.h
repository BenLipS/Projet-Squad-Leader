#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_ServerWaitForClientTargetData.h"
#include "SquadLeader/Weapons/SL_Weapon.h"
#include "../GameplayEffects/States/GE_StateFiring.h"
#include "GA_FireWeaponInstant.generated.h"

UCLASS()
class SQUADLEADER_API UGA_FireWeaponInstant : public UGameplayAbilitySoldier
{
	GENERATED_BODY()
	
public:
	UGA_FireWeaponInstant();

	UFUNCTION(BlueprintCallable)
	void FireBullet();

	UFUNCTION(BlueprintCallable)
	void HandleTargetData(const FGameplayAbilityTargetDataHandle& _Data);

protected:
	void ApplyDamagesAndHits(const FGameplayAbilityTargetDataHandle& _Data, ASoldier* _SourceSoldier);

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayEffect> GE_DamageClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGE_StateFiring> GE_FiringStateClass;

	// TODO: Make a fname variable to change the type of collision to detect
	//// CollisionChannel the weapon should detect
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	//TEnumAsByte<ECollisionChannel> TraceProfile;

	// Cache values
	USL_ServerWaitForClientTargetData* ServerWaitForClientTargetDataTask;
	ASL_Weapon* SourceWeapon;
	float TimeOfLastShoot;
};
