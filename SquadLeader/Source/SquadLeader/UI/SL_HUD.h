#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Interface/PlayerHealthDelegateInterface.h"
#include "Interface/PlayerShieldDelegateInterface.h"
#include "Interface/PlayerPrestigeDelegateInterface.h"
#include "Interface/SquadDelegateInterface.h"
#include "Interface/WeaponDelegateInterface.h"
#include "Interface/OrderDelegateInterface.h"
#include "Interface/NotificationDelegateInterface.h"
#include "Interface/StatInfoDelegateInterface.h"
#include "Interface/MinimapDelegateInterface.h"
#include "SL_HUD.generated.h"

UCLASS()
class SQUADLEADER_API ASL_HUD : public AHUD,
	public IPlayerHealthDelegateInterface,
	public IPlayerShieldDelegateInterface,
	public IPlayerPrestigeDelegateInterface,
	public ISquadDelegateInterface,
	public IWeaponDelegateInterface,
	public IOrderDelegateInterface,
	public INotificationDelegateInterface,
	public IStatInfoDelegateInterface,
	public IMinimapDelegateInterface

{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TArray<TSubclassOf<class USL_UserWidget>> WidgetClassList;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TArray<USL_UserWidget*> WidgetList;

public:
	ASL_HUD() = default;

protected:
	virtual void BeginPlay() override;

public:
	/*Initialize all data from Owner controller PlayerState,
	should be called when changing player controller to relink datas*/
	void SetPlayerStateLink();

	void SetAIStateLink();

	void BindSoldierTeamChanges();
	void BindControlAreas();
	void GetProfileData();

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

//-----IStatInfoDelegateInterface-----
public:
	//void OnStatInfoReceived(FString Key, FString Value) override;
	//void OnStatsInfoReceived(TMap<FString, FString> statsIn) override;
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
};