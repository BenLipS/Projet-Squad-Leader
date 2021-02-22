#include "GE_State.h"

UGE_State::UGE_State(const FGameplayTag& _Tag) : Super()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(_Tag);
}

UGE_State::UGE_State(const FGameplayTag& _Tag, const float _Duration) : Super()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(_Duration));
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(_Tag);
}