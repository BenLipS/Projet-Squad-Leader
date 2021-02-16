#include "GE_StateRunning.h"

UGE_StateRunning::UGE_StateRunning() : UGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Running")));
}