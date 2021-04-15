// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#define ECC_Player ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel2

UENUM(BlueprintType)
enum class ESoldierAbilityInputID : uint8
{
	// None
	None			UMETA(DisplayName = "None"),
	// LMB
	ConfirmSkill	UMETA(DisplayName = "ConfirmSkill"),
	// RMB
	CancelSkill		UMETA(DisplayName = "CancelSkill"),
	// Shift
	Run				UMETA(DisplayName = "Run"),
	// Left Ctrl
	Crouch			UMETA(DisplayName = "Crouch"),
	// Space
	Jump			UMETA(DisplayName = "Jump"),
	// LMB
	BasicAttack		UMETA(DisplayName = "BasicAttack"),
	// MMB
	MeleeAttack		UMETA(DisplayName = "MeleeAttack"),
	// RMB
	Aim				UMETA(DisplayName = "Aim"),
	// Q
	Ability1		UMETA(DisplayName = "Ability1"),
	// E
	Ability2		UMETA(DisplayName = "Ability2"),
	// T
	GiveOrder		UMETA(DisplayName = "GiveOrder"),
	// R
	ReloadWeapon	UMETA(DisplayName = "ReloadWeapon"),
	// Left Alt
	QuickDash		UMETA(DisplayName = "QuickDash"),
	// P
	WallVision		UMETA(DisplayName = "WallVision"),
};