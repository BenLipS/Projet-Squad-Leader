#include "SoldierMovementComponent.h"
#include "Soldier.h"
#include "../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../AbilitySystem/Soldiers/AttributeSetSoldier.h"

USoldierMovementComponent::USoldierMovementComponent(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
}

float USoldierMovementComponent::GetMaxSpeed() const
{
    ASoldier* soldier = Cast<ASoldier>(GetCharacterOwner());
    UAbilitySystemSoldier* ASC = soldier ? soldier->GetAbilitySystemComponent() : nullptr;

    float finalSpeed = 0.f;

    switch (MovementMode)
    {
    case MOVE_Walking:
    case MOVE_NavWalking:
        if (IsCrouching())
            finalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedCrouchAttribute()) : MaxWalkSpeedCrouched;
        else
            finalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedWalkAttribute()) : MaxWalkSpeed;
        break;

    case MOVE_Falling:
        finalSpeed = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedWalkAttribute()) : MaxWalkSpeed;
        break;

    case MOVE_Swimming:
        finalSpeed = MaxSwimSpeed;
        break;

    case MOVE_Flying:
        finalSpeed = MaxFlySpeed;
        break;

    case MOVE_Custom:
        finalSpeed = MaxCustomMovementSpeed;
        break;

    case MOVE_None:
    default:
        finalSpeed = 0.f;
        break;
    }

    const float moveSpeedMultiplier = ASC ? ASC->GetNumericAttribute(UAttributeSetSoldier::GetMoveSpeedMultiplierAttribute()) : 1.f;
    return finalSpeed * moveSpeedMultiplier;
}