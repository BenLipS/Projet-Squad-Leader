#include "DamageExecCalculation.h"
#include "AbilitySystemSoldier.h"
#include "AttributeSetSoldier.h"

struct _DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	_DamageStatics()
	{
		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetSoldier, Damage, Source, true);
	}
};

static const _DamageStatics& DamageStatics()
{
	static _DamageStatics DStatics;
	return DStatics;
}

UDamageExecCalculation::UDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Get tags from SourceActor and TargetActor - to handle buffs/debuffs
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.0f;

	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

	// Add SetByCaller damage if exists
	//Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);
	Damage += Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, 0.0f);

	// Set Damage attribut to the target
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, Damage));
}
