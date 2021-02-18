// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_Default_Stats_AreaEffect.h"
#include "AttributeSetAreaEffect.h"

UGE_Default_Stats_AreaEffect::UGE_Default_Stats_AreaEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	ChanceToApplyToTarget = FScalableFloat(1.0f);

	// Duration
	FGameplayModifierInfo modifierDuration;
	modifierDuration.Attribute = UAttributeSetAreaEffect::GetDurationAttribute();
	modifierDuration.ModifierOp = EGameplayModOp::Override;
	modifierDuration.ModifierMagnitude = FScalableFloat(0.f); // instant one use
	Modifiers.Add(modifierDuration);

	// Radius
	FGameplayModifierInfo modifierRadius;
	modifierRadius.Attribute = UAttributeSetAreaEffect::GetRadiusAttribute();
	modifierRadius.ModifierOp = EGameplayModOp::Override;
	modifierRadius.ModifierMagnitude = FScalableFloat(500.f); // 5 metter
	Modifiers.Add(modifierRadius);

	// Interval
	FGameplayModifierInfo modifierInterval;
	modifierInterval.Attribute = UAttributeSetAreaEffect::GetIntervalAttribute();
	modifierInterval.ModifierOp = EGameplayModOp::Override;
	modifierInterval.ModifierMagnitude = FScalableFloat(0.1f); // 5 metter
	Modifiers.Add(modifierInterval);
}