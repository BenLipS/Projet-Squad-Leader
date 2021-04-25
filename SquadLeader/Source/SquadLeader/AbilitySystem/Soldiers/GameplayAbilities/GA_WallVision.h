#pragma once

#include "CoreMinimal.h"
#include "../GameplayAbilitySoldier.h"
#include "GA_WallVision.generated.h"

class ASoldierPlayer;

UCLASS()
class SQUADLEADER_API UGA_WallVision : public UGameplayAbilitySoldier
{
	GENERATED_BODY()

public:
	UGA_WallVision();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void MontageCompletedOrBlendedOut();

	UFUNCTION()
	void MontageInterruptedOrCancelled();

	UFUNCTION()
	void MontageSentEvent(FGameplayTag _EventTag, FGameplayEventData _EventData);

	UFUNCTION()
	void StartWallVision();

	UFUNCTION()
	void EndWallVision();

	UPROPERTY(EditDefaultsOnly)
	float DurationVision = 8.f;

	// Event tag to notify from the montage to wall vision
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallVisionReadyEventTag;

	// Cache values
	ASoldierPlayer* SourceSoldier = nullptr;
};