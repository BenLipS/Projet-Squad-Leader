#include "GE_UpdateStats.h"
#include "../AttributeSetSoldier.h"

UGE_UpdateStats::UGE_UpdateStats()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	ChanceToApplyToTarget = FScalableFloat(1.0f);

	// Max Health
	FGameplayModifierInfo ModifierMaxHealth;
	ModifierMaxHealth.Attribute = UAttributeSetSoldier::GetMaxHealthAttribute();
	ModifierMaxHealth.ModifierOp = EGameplayModOp::Override;
	ModifierMaxHealth.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(ModifierMaxHealth);

	// Max Shield
	FGameplayModifierInfo ModifierMaxShield;
	ModifierMaxShield.Attribute = UAttributeSetSoldier::GetMaxShieldAttribute();
	ModifierMaxShield.ModifierOp = EGameplayModOp::Override;
	ModifierMaxShield.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(ModifierMaxShield);

	// Health Regen Rate
	FGameplayModifierInfo ModifierHealthRegenRate;
	ModifierHealthRegenRate.Attribute = UAttributeSetSoldier::GetHealthRegenRateAttribute();
	ModifierHealthRegenRate.ModifierOp = EGameplayModOp::Override;
	ModifierHealthRegenRate.ModifierMagnitude = FScalableFloat(10.f);
	Modifiers.Add(ModifierHealthRegenRate);

	// Move Speed Walk
	FGameplayModifierInfo ModifierMoveSpeedWalk;
	ModifierMoveSpeedWalk.Attribute = UAttributeSetSoldier::GetMoveSpeedWalkAttribute();
	ModifierMoveSpeedWalk.ModifierOp = EGameplayModOp::Override;
	ModifierMoveSpeedWalk.ModifierMagnitude = FScalableFloat(600.f);
	Modifiers.Add(ModifierMoveSpeedWalk);

	// Move Speed Crouch
	FGameplayModifierInfo ModifierMoveSpeedCrouch;
	ModifierMoveSpeedCrouch.Attribute = UAttributeSetSoldier::GetMoveSpeedCrouchAttribute();
	ModifierMoveSpeedCrouch.ModifierOp = EGameplayModOp::Override;
	ModifierMoveSpeedCrouch.ModifierMagnitude = FScalableFloat(200.f);
	Modifiers.Add(ModifierMoveSpeedCrouch);

	// Move Speed Multiplier
	FGameplayModifierInfo ModifierMoveSpeedMultiplier;
	ModifierMoveSpeedMultiplier.Attribute = UAttributeSetSoldier::GetMoveSpeedMultiplierAttribute();
	ModifierMoveSpeedMultiplier.ModifierOp = EGameplayModOp::Override;
	ModifierMoveSpeedMultiplier.ModifierMagnitude = FScalableFloat(1.f);
	Modifiers.Add(ModifierMoveSpeedMultiplier);
}