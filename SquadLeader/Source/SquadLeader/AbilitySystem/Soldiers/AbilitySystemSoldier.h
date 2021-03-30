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
};
