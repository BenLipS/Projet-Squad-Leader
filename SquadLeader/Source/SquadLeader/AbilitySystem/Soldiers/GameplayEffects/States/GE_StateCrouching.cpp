#include "GE_StateCrouching.h"

UGE_StateCrouching::UGE_StateCrouching()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Crouching")));
}