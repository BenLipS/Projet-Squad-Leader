// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentAreaEffect.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAbilitySystemComponentAreaEffect : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UAbilitySystemComponentAreaEffect() = default;

	bool startupEffectsApplied = false;
};
