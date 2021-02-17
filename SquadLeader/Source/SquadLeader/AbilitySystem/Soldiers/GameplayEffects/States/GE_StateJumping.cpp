#include "GE_StateJumping.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateJumping::UGE_StateJumping() : UGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(ASoldier::StateJumpingTag);
}