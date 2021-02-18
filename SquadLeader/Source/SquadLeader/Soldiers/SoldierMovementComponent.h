#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SoldierMovementComponent.generated.h"

UCLASS()
class SQUADLEADER_API USoldierMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USoldierMovementComponent(const FObjectInitializer& _ObjectInitializer);
	virtual float GetMaxSpeed() const override;
};
