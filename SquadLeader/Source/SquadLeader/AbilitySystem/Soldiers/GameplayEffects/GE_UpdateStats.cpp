#include "GE_UpdateStats.h"
#include "../AttributeSetSoldier.h"

UGE_UpdateStats::UGE_UpdateStats()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	ChanceToApplyToTarget = FScalableFloat(1.0f);

	// Max Health
	FGameplayModifierInfo ModifierMaxHealth;
	ModifierMaxHealth.Attribute = UAttributeSetSoldier::GetMaxHealthAttribute();
	ModifierMaxHealth.ModifierOp = EGameplayModOp::Additive;
	ModifierMaxHealth.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(ModifierMaxHealth);

	// Max Shield
	FGameplayModifierInfo ModifierMaxShield;
	ModifierMaxShield.Attribute = UAttributeSetSoldier::GetMaxShieldAttribute();
	ModifierMaxShield.ModifierOp = EGameplayModOp::Additive;
	ModifierMaxShield.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(ModifierMaxShield);

	// Health Regen Rate
	FGameplayModifierInfo ModifierHealthRegenRate;
	ModifierHealthRegenRate.Attribute = UAttributeSetSoldier::GetHealthRegenRateAttribute();
	ModifierHealthRegenRate.ModifierOp = EGameplayModOp::Additive;
	ModifierHealthRegenRate.ModifierMagnitude = FScalableFloat(10.f);
	Modifiers.Add(ModifierHealthRegenRate);

	// Move Speed Walk
	FGameplayModifierInfo ModifierMoveSpeedWalk;
	ModifierMoveSpeedWalk.Attribute = UAttributeSetSoldier::GetMoveSpeedWalkAttribute();
	ModifierMoveSpeedWalk.ModifierOp = EGameplayModOp::Additive;
	ModifierMoveSpeedWalk.ModifierMagnitude = FScalableFloat(600.f);
	Modifiers.Add(ModifierMoveSpeedWalk);

	// Move Speed Crouch
	FGameplayModifierInfo ModifierMoveSpeedCrouch;
	ModifierMoveSpeedCrouch.Attribute = UAttributeSetSoldier::GetMoveSpeedCrouchAttribute();
	ModifierMoveSpeedCrouch.ModifierOp = EGameplayModOp::Additive;
	ModifierMoveSpeedCrouch.ModifierMagnitude = FScalableFloat(200.f);
	Modifiers.Add(ModifierMoveSpeedCrouch);

	// EXP Level up - What you need to level up
	FGameplayModifierInfo ModifierEXPLevelUp;
	ModifierEXPLevelUp.Attribute = UAttributeSetSoldier::GetEXPLevelUpAttribute();
	ModifierEXPLevelUp.ModifierOp = EGameplayModOp::Additive;
	ModifierEXPLevelUp.ModifierMagnitude = FScalableFloat(100.f);
	Modifiers.Add(ModifierEXPLevelUp);
}