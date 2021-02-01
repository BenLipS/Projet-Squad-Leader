// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class ESoldierAbilityInputID : uint8
{
	// None
	None			UMETA(DisplayName = "None"),
	// Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// Shift
	Run				UMETA(DisplayName = "Run"),
	// C
	Crouch			UMETA(DisplayName = "Crouch"),
	// Space
	Jump			UMETA(DisplayName = "Jump"),
	// LMB
	BasicAttack		UMETA(DisplayName = "BasicAttack"),
	// MMB
	MeleeAttack		UMETA(DisplayName = "MeleeAttack"),
	// Q
	Ability1		UMETA(DisplayName = "Ability1"),
	// E
	Ability2		UMETA(DisplayName = "Ability2"),
};