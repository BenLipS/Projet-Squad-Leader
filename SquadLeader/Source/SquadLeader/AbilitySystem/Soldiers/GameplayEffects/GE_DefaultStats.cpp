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

	// Max Health
	FGameplayModifierInfo modifierMaxHealth;
	modifierMaxHealth.Attribute = UAttributeSetSoldier::GetMaxHealthAttribute();
	modifierMaxHealth.ModifierOp = EGameplayModOp::Override;
	modifierMaxHealth.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(modifierMaxHealth);

	// Health
	FGameplayModifierInfo modifierHealth;
	modifierHealth.Attribute = UAttributeSetSoldier::GetHealthAttribute();
	modifierHealth.ModifierOp = EGameplayModOp::Override;
	modifierHealth.ModifierMagnitude = modifierMaxHealth.ModifierMagnitude;
	Modifiers.Add(modifierHealth);

	// Max Shield
	FGameplayModifierInfo modifierMaxShield;
	modifierMaxShield.Attribute = UAttributeSetSoldier::GetMaxShieldAttribute();
	modifierMaxShield.ModifierOp = EGameplayModOp::Override;
	modifierMaxShield.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(modifierMaxShield);

	// Shield
	FGameplayModifierInfo modifierShield;
	modifierShield.Attribute = UAttributeSetSoldier::GetShieldAttribute();
	modifierShield.ModifierOp = EGameplayModOp::Override;
	modifierShield.ModifierMagnitude = modifierMaxShield.ModifierMagnitude;
	Modifiers.Add(modifierShield);

	// Health Regen Rate
	FGameplayModifierInfo modifierHealthRegenRate;
	modifierHealthRegenRate.Attribute = UAttributeSetSoldier::GetHealthRegenRateAttribute();
	modifierHealthRegenRate.ModifierOp = EGameplayModOp::Override;
	modifierHealthRegenRate.ModifierMagnitude = FScalableFloat(10.f);
	Modifiers.Add(modifierHealthRegenRate);

	// Move Speed Walk
	FGameplayModifierInfo modifierMoveSpeedWalk;
	modifierMoveSpeedWalk.Attribute = UAttributeSetSoldier::GetMoveSpeedWalkAttribute();
	modifierMoveSpeedWalk.ModifierOp = EGameplayModOp::Override;
	modifierMoveSpeedWalk.ModifierMagnitude = FScalableFloat(600.f);
	Modifiers.Add(modifierMoveSpeedWalk);

	// Move Speed Crouch
	FGameplayModifierInfo modifierMoveSpeedCrouch;
	modifierMoveSpeedCrouch.Attribute = UAttributeSetSoldier::GetMoveSpeedCrouchAttribute();
	modifierMoveSpeedCrouch.ModifierOp = EGameplayModOp::Override;
	modifierMoveSpeedCrouch.ModifierMagnitude = FScalableFloat(200.f);
	Modifiers.Add(modifierMoveSpeedCrouch);

	// Move Speed Multiplier
	FGameplayModifierInfo modifierMoveSpeedMultiplier;
	modifierMoveSpeedMultiplier.Attribute = UAttributeSetSoldier::GetMoveSpeedMultiplierAttribute();
	modifierMoveSpeedMultiplier.ModifierOp = EGameplayModOp::Override;
	modifierMoveSpeedMultiplier.ModifierMagnitude = FScalableFloat(1.f);
	Modifiers.Add(modifierMoveSpeedMultiplier);
}