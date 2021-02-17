#include "GE_StateCrouching.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateCrouching::UGE_StateCrouching() : UGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(ASoldier::StateCrouchingTag);
}