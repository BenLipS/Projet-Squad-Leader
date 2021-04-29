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

	/**
	 * Returns degree of the angle betwee velocity and Rotation forward vector
	 * The range of return will be from [-180, 180], and this can be used to feed blendspace directional value
	 */
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation) const;

protected:
	// Determine the current movespeed multiplier based on the direction of the soldier
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement (General Settings)")
	UCurveFloat* CurveMoveSpeedMultiplierDirection;
};
