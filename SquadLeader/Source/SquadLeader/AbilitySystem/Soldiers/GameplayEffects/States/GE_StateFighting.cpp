#include "GE_StateFighting.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateFighting::UGE_StateFighting() : UGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(10.f));
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(ASoldier::StateFightingTag);
}