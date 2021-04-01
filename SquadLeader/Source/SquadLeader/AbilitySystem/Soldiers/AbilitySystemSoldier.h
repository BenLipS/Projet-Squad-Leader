#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemSoldier.generated.h"

UCLASS()
class SQUADLEADER_API UAbilitySystemSoldier : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAbilitySystemSoldier() = default;

	bool bStartupEffectsApplied = false;

	virtual bool ShouldDoServerAbilityRPCBatch() const override { return true; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle _InAbilityHandle, bool _EndAbilityImmediately);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> _AbilityClass, UObject* _OptionalSourceObject = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	UGameplayAbilitySoldier* GetPrimaryAbilityInstanceFromHandle(FGameplayAbilitySpecHandle _Handle);

//////////////// GameplayCue
	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters);

//////////////// Cooldowns
	UFUNCTION(BlueprintCallable, Category = "Cooldowns")
	bool GetCooldownRemainingForTags(const FGameplayTagContainer& _CooldownTags, float& _TimeRemaining);

	UFUNCTION(BlueprintCallable, Category = "Cooldowns")
	bool GetCooldownRemainingForTag(const FGameplayTag& _CooldownTags, float& _TimeRemaining);

	UFUNCTION(BlueprintCallable, Category = "Cooldowns")
	bool GetCooldownRemainingAndDurationForTags(const FGameplayTagContainer& _CooldownTags, float& _TimeRemaining, float& _CooldownDuration);

	UFUNCTION(BlueprintCallable, Category = "Cooldowns")
	bool GetCooldownRemainingAndDurationForTag(const FGameplayTag& _CooldownTags, float& _TimeRemaining, float& _CooldownDuration);
};
