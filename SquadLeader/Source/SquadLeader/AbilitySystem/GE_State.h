#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_State.generated.h"

UCLASS()
class SQUADLEADER_API UGE_State : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_State() = default;
	UGE_State(const FGameplayTag& _Tag);
	UGE_State(const FGameplayTag& _Tag, const float _Duration);
};
