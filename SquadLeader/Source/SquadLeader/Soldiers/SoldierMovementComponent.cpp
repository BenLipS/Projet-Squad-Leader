#include "SoldierMovementComponent.h"
#include "Soldier.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../AbilitySystem/Soldiers/AttributeSetSoldier.h"

USoldierMovementComponent::USoldierMovementComponent(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
}

float USoldierMovementComponent::GetMaxSpeed() const
{
	ASoldier* Soldier = Cast<ASoldier>(GetCharacterOwner());

	if (!Soldier)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Soldier as owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Soldier->IsAlive())
		return 0.f;

	UAbilitySystemSoldier* ASC = Soldier ? Soldier->GetAbilitySystemComponent() : nullptr;
	float FinalSpeed = 0.f;

	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	{
		const float AngleDirection = CalculateDirection(Soldier->GetVelocity(), Soldier->GetActorRotation());
		const float MoveSpeedDirectionMultiplier = CurveMoveSpeedMultiplierDirection ? CurveMoveSpeedMultiplierDirection->GetFloatValue(AngleDirection) : 1.f;

		if (IsCrouching())
			FinalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedCrouchAttribute()) : MaxWalkSpeedCrouched;
		else
			FinalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedWalkAttribute()) : MaxWalkSpeed;

		FinalSpeed *= MoveSpeedDirectionMultiplier;
		break;
	}

	case MOVE_Falling:
		FinalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedWalkAttribute()) : MaxWalkSpeed;
		break;

	case MOVE_Swimming:
		FinalSpeed = MaxSwimSpeed;
		break;

	case MOVE_Flying:
		FinalSpeed = MaxFlySpeed;
		break;

	case MOVE_Custom:
		FinalSpeed = MaxCustomMovementSpeed;
		break;

	case MOVE_None:
	default:
		FinalSpeed = 0.f;
		break;
	}

	const float MoveSpeedMultiplier = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedMultiplierAttribute()) : 1.f;
	return FinalSpeed * MoveSpeedMultiplier;
}

float USoldierMovementComponent::CalculateDirection(const FVector& _Velocity, const FRotator& _BaseRotation) const
{
	if (!_Velocity.IsNearlyZero())
	{
		const FMatrix RotMatrix = FRotationMatrix(_BaseRotation);
		const FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		const FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		const FVector NormalizedVel = _Velocity.GetSafeNormal2D();

		// get a cos(alpha) of forward vector vs velocity
		const float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
		// now get the alpha and convert to degree
		float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

		// depending on where right vector is, flip it
		const float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);

		return (RightCosAngle < 0) ? -ForwardDeltaDegree : ForwardDeltaDegree;
	}
	return 0.f;
}