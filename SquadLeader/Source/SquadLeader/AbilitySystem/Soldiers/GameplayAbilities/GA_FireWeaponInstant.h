#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "SquadLeader/AbilitySystem/Soldiers/AbilityTasks/SL_ServerWaitForClientTargetData.h"
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
	void ApplyEffectsToSource();
	void ApplyDamages(const FGameplayAbilityTargetDataHandle& _Data, const FGameplayEffectSpecHandle& _DamageEffectSpecHandle, UAbilitySystemComponent* _TargetASC);
	void ApplyDamages(class AShield* _Shield, const float _Damages);

	void ReloadWeapon();
	void ConfigLineTrace();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayEffect> GE_DamageClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGE_StateFiring> GE_FiringStateClass;

	// Cache values
	USL_ServerWaitForClientTargetData* ServerWaitForClientTargetDataTask;
	class ASL_Weapon* SourceWeapon;
	class ASoldier* SourceSoldier;
	class ASL_LineTrace* LineTrace;
	float TimeOfLastShoot;
};
