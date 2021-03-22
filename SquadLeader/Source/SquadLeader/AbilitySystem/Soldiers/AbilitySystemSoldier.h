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

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle _InAbilityHandle, bool _EndAbilityImmediately);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject = nullptr);
};
