#include "GE_StateRunning.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateRunning::UGE_StateRunning() : UGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	ChanceToApplyToTarget = FScalableFloat(1.0f);
	InheritableOwnedTagsContainer.AddTag(ASoldier::StateRunningTag);
}