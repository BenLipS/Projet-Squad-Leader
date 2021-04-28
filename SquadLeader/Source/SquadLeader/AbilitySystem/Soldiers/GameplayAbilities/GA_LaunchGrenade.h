#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "../../../Projectile/SL_Projectile.h"
#include "GA_LaunchGrenade.generated.h"

class ASoldier;

UCLASS()
class SQUADLEADER_API UGA_LaunchGrenade : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_LaunchGrenade();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void MontageCompletedOrBlendedOut();

	UFUNCTION()
	void MontageInterruptedOrCancelled();

	UFUNCTION()
	void MontageSentEvent(FGameplayTag _EventTag, FGameplayEventData _EventData);

	UFUNCTION()
	void ThrowProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASL_Projectile> ProjectileClass;

	// If true the projectile will be spawned from right hand. Else from left hand
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	bool bThrowWithRightHand = true;

	// Event tag to notify from the montage to launch the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FGameplayTag ThrownProjectileEventTag;

	// Cache values
	ASoldier* SourceSoldier = nullptr;
};