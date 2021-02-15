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

	bool startupEffectsApplied = false;
};
