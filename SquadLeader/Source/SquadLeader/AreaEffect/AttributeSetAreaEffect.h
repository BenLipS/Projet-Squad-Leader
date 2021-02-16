#pragma once

#include "Core.h"
#include "UObject/ObjectMacros.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AttributeSetAreaEffect.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SQUADLEADER_API UAttributeSetAreaEffect : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAttributeSetAreaEffect() = default;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute & Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data) override;

	// Duration
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Duration", ReplicatedUsing = OnRep_Duration)
	FGameplayAttributeData Duration;
	ATTRIBUTE_ACCESSORS(UAttributeSetAreaEffect, Duration)

protected:
	UFUNCTION()
	virtual void OnRep_Duration(const FGameplayAttributeData& OldDuration);
};