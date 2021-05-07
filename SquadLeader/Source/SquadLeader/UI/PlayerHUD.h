// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_HUD.h"
#include "Interface/PlayerHealthDelegateInterface.h"
#include "Interface/PlayerShieldDelegateInterface.h"
#include "Interface/PlayerPrestigeDelegateInterface.h"
#include "Interface/SquadDelegateInterface.h"
#include "Interface/WeaponDelegateInterface.h"
#include "Interface/OrderDelegateInterface.h"
#include "Interface/NotificationDelegateInterface.h"
#include "Interface/StatInfoDelegateInterface.h"
#include "Interface/MinimapDelegateInterface.h"
#include "Interface/TicketDelegateInterface.h"
#include "Interface/GameEndDelegateInterface.h"
#include "Interface/AbilityCooldownDelegateInterface.h"
#include "Interface/HitMarkerInterfaceDelegate.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API APlayerHUD : public ASL_HUD
{
	GENERATED_BODY()
	
};
