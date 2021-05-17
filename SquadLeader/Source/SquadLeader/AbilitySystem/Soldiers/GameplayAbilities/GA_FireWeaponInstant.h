#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "../GameplayEffects/States/GE_StateFiring.h"
#include "GA_FireWeaponInstant.generated.h"

class USL_ServerWaitForClientTargetData;
class UAbilityTask_WaitDelay;
class ASoldier;

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

	UFUNCTION(BlueprintImplementableEvent)
	void DoAnimation(const FGameplayCueParameters _Parameters);

protected:
	void ApplyEffectsToSource();
	bool ApplyDamages(const FGameplayAbilityTargetDataHandle& _Data, ASoldier* _TargetSoldier, const float _Damage);
	bool ApplyDamages(class AShield* _Shield, const float _Damage);

	bool IsHeadShot(const FHitResult& _HitResult) const;

	void ReloadWeapon();
	void ConfigLineTrace();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayEffect> GE_DamageClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TSubclassOf<UGE_StateFiring> GE_FiringStateClass;

	// Cache values
	USL_ServerWaitForClientTargetData* ServerWaitForClientTargetDataTask;
	UAbilityTask_WaitDelay* TaskWaitDelay;
	class ASL_Weapon* SourceWeapon;
	class ASoldier* SourceSoldier;
	class ASL_LineTrace* LineTrace;
	float TimeOfLastShoot;
};
