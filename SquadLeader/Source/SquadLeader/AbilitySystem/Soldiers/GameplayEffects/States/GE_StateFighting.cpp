#include "GE_StateFighting.h"

UGE_StateFighting::UGE_StateFighting()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(10.f));
	ChanceToApplyToTarget = FScalableFloat(1.0f);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Fighting")));
}