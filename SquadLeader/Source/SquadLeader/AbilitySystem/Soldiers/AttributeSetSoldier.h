#pragma once

#include "Core.h"
#include "UObject/ObjectMacros.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AttributeSetSoldier.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SQUADLEADER_API UAttributeSetSoldier : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAttributeSetSoldier();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Character level
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Character Level", ReplicatedUsing = OnRep_CharacterLevel)
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, CharacterLevel)

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, Damage)

	// Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, Health)

	// Max health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | MaxHealth", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, MaxHealth)

	// Health regen
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | HealthRegenRate", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, HealthRegenRate)

	// Move Speed
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UAttributeSetSoldier, MoveSpeed)

protected:
	UFUNCTION()
	virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
};
