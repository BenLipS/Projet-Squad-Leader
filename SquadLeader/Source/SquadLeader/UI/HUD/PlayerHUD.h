// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SL_HUD.h"
#include "../Interface/PlayerHealthDelegateInterface.h"
#include "../Interface/PlayerShieldDelegateInterface.h"
#include "../Interface/PlayerPrestigeDelegateInterface.h"
#include "../Interface/SquadDelegateInterface.h"
#include "../Interface/WeaponDelegateInterface.h"
#include "../Interface/OrderDelegateInterface.h"
#include "../Interface/NotificationDelegateInterface.h"
#include "../Interface/MinimapDelegateInterface.h"
#include "../Interface/TicketDelegateInterface.h"
#include "../Interface/GameEndDelegateInterface.h"
#include "../Interface/AbilityCooldownDelegateInterface.h"
#include "../Interface/HitMarkerInterfaceDelegate.h"
#include "../Interface/ChatDelegateInterface.h"
#include "../Interface/RespawnInterface.h"

#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API APlayerHUD : public ASL_HUD,
	public IPlayerHealthDelegateInterface,
	public IPlayerShieldDelegateInterface,
	public IPlayerPrestigeDelegateInterface,
	public ISquadDelegateInterface,
	public IWeaponDelegateInterface,
	public IOrderDelegateInterface,
	public INotificationDelegateInterface,
	public IMinimapDelegateInterface,
	public ITicketDelegateInterface,
	public IGameEndDelegateInterface,
	public IAbilityCooldownDelegateInterface,
	public IHitMarkerInterfaceDelegate,
	public IChatDelegateInterface,
	public IRespawnInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	virtual void OnInitHUD() override;
	
public:
	/*Initialize all data from Owner controller PlayerState,
	should be called when changing player controller to relink datas*/
	void SetPlayerStateLink();

	void SetAIStateLink();

	void BindSoldierTeamChanges();
	void BindControlAreas();

	/*UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DisplayRespawnWidget();*/

	/*********************************
*****INTERFACE IMPLEMENTATION*****
*********************************/

//-----IPlayerHealthDelegateInterface-----
public:
	//void OnPlayerHealthChanged(float newValue) override;
	//void OnPlayerMaxHealthChanged(float newValue) override;


//-----IPlayerShieldDelegateInterface-----
public:
	//void OnPlayerShieldChanged(float newValue) override;
	//void OnPlayerMaxShieldChanged(float newValue) override;


//-----ISquadDelegateInterface-----
public:
	//void OnSquadChanged(const TArray<FSoldierAIData>& newValue) override;
	//void OnSquadHealthChanged(int index, float newHealth) override;
	//void OnSquadMaxHealthChanged(int index, float newMaxHealth) override;
	//void OnSquadShieldChanged(int index, float newShield) override;
	//void OnSquadMaxShieldChanged(int index, float newMaxShield) override;

//-----ISquadDelegateInterface-----
public:
	//void OnAmmoChanged(uint8 newValue) override;
	//void OnMaxAmmoChanged(uint8 newValue) override;

//-----IOrderDelegateInterface-----
public:
	//void OnOrderInputPressed() override;
	//void OnOrderInputReleased() override;

//-----INotificationDelegateInterface-----
public:
	//void OnTextNotification_Received(FText textNotification) override;

//-----IMinimapInterface-----
public:
	//void OnSoldierAddedToTeam(ASoldier* _Soldier) override;
	//void OnSoldierRemovedFromTeam() override;
	//void OnUpdatePOIs() override;
	//void OnControlAreaAdded() override;

//-----IPlayerPrestigeDelegateInterface-----
public:
	//void OnPlayerPrestigeChanged(float newValue) override;
	//void OnPlayerPrestigeLevelUpChanged(float newValue) override;

//-----IControlAreaDelegateInterface-----
public:
	//virtual void OnControlAreaInit(unsigned int nbArea) override;
	//virtual void OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent) override;

//-----IGameEndDelegateInterface-----
public:
	//virtual void OnGameEnd(FString EndMessage) override;

//-----IAbilityCooldownDelegateInterface-----
public:
	//virtual void OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key) override;
	//virtual void AddAbilityID(ESoldierAbilityInputID Key) override;

public:
	//-----IHitMarkerInterfaceDelegate-----

//-----IChatDelegateItnerface-----
public:
	//void OnChatMessageReceived(const FString& message) override;
};
