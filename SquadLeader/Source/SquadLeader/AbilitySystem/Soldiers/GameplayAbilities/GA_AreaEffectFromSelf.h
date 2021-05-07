#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "../../../AreaEffect/AreaEffect.h"
#include "GA_AreaEffectFromSelf.generated.h"

UCLASS()
class SQUADLEADER_API UGA_AreaEffectFromSelf : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_AreaEffectFromSelf();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AAreaEffect> AreaEffectClass;

//////////////// Collision
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTraceAreaEffect = false;

//Effect
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Particles")
	TSubclassOf<AActor> AreaFXClass;

	UPROPERTY()
	AActor* AreaFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Particles")
	bool DoesEffectFollowActor = false;
		
};
