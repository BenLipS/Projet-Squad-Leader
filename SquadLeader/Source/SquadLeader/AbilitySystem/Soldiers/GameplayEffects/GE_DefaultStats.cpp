#include "GE_DefaultStats.h"
#include "../AttributeSetSoldier.h"

UGE_DefaultStats::UGE_DefaultStats()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	ChanceToApplyToTarget = FScalableFloat(1.0f);

	// Level
	FGameplayModifierInfo modifierLvl;
	modifierLvl.Attribute = UAttributeSetSoldier::GetCharacterLevelAttribute();
	modifierLvl.ModifierOp = EGameplayModOp::Override;
	modifierLvl.ModifierMagnitude = FScalableFloat(1.0f);
	Modifiers.Add(modifierLvl);

	// Health
	FGameplayModifierInfo modifierHealth;
	modifierHealth.Attribute = UAttributeSetSoldier::GetHealthAttribute();
	modifierHealth.ModifierOp = EGameplayModOp::Override;
	modifierHealth.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(modifierHealth);

	// Max Health
	FGameplayModifierInfo modifierMaxHealth;
	modifierMaxHealth.Attribute = UAttributeSetSoldier::GetMaxHealthAttribute();
	modifierMaxHealth.ModifierOp = EGameplayModOp::Override;
	modifierMaxHealth.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(modifierMaxHealth);

	// Health Regen Rate
	FGameplayModifierInfo modifierHealthRegenRate;
	modifierHealthRegenRate.Attribute = UAttributeSetSoldier::GetHealthRegenRateAttribute();
	modifierHealthRegenRate.ModifierOp = EGameplayModOp::Override;
	modifierHealthRegenRate.ModifierMagnitude = FScalableFloat(10.f);
	Modifiers.Add(modifierHealthRegenRate);

	// Move Speed
	FGameplayModifierInfo modifierMoveSpeed;
	modifierMoveSpeed.Attribute = UAttributeSetSoldier::GetMoveSpeedAttribute();
	modifierMoveSpeed.ModifierOp = EGameplayModOp::Override;
	modifierMoveSpeed.ModifierMagnitude = FScalableFloat(600.f);
	Modifiers.Add(modifierMoveSpeed);
}