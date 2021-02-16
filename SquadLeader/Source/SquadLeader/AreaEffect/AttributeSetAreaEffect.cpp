// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetAreaEffect.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

void UAttributeSetAreaEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
}

bool UAttributeSetAreaEffect::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return false;
}

void UAttributeSetAreaEffect::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

void UAttributeSetAreaEffect::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}

void UAttributeSetAreaEffect::OnRep_Duration(const FGameplayAttributeData& OldDuration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetAreaEffect, Duration, OldDuration);
}