#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "../../../Projectile/SL_Projectile.h"
#include "GA_LaunchGrenade.generated.h"

UCLASS()
class SQUADLEADER_API UGA_LaunchGrenade : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_LaunchGrenade();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ASL_Projectile> MyProjectile;
};