// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetAreaEffect.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

void UAttributeSetAreaEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeSetAreaEffect, Duration);
	DOREPLIFETIME(UAttributeSetAreaEffect, Radius);
	DOREPLIFETIME(UAttributeSetAreaEffect, Interval);
}

bool UAttributeSetAreaEffect::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UAttributeSetAreaEffect::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAttributeSetAreaEffect::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}

void UAttributeSetAreaEffect::OnRep_Duration(const FGameplayAttributeData& OldDuration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetAreaEffect, Duration, OldDuration);
}

void UAttributeSetAreaEffect::OnRep_Radius(const FGameplayAttributeData& OldRadius)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetAreaEffect, Radius, OldRadius);
}

void UAttributeSetAreaEffect::OnRep_Interval(const FGameplayAttributeData& OldInterval)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetAreaEffect, Interval, OldInterval);
}