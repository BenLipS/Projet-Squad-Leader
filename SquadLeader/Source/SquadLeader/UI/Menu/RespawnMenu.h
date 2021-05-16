// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuWidget.h"

#include "SquadLeader/UI/Interface/MinimapDelegateInterface.h"
#include "SquadLeader/UI/Interface/RespawnInterface.h"

#include "RespawnMenu.generated.h"

class UMenuLayoutWidget;
class URespawnMenuLayout;
class UMenuButtonWidget;
class UMenuButtonControlAreaWidget;

UCLASS()
class SQUADLEADER_API URespawnMenu : public UMenuWidget,
	public IMinimapDelegateInterface,
	public IRespawnInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UMenuButtonControlAreaWidget> RespawnButtonClass;

	UPROPERTY(BlueprintReadOnly, meta =(BindWidget))
	URespawnMenuLayout* LayoutRespawn;

	TArray<UMenuButtonControlAreaWidget*> ControlAreaButtonList;


protected:
	virtual void SynchronizeProperties();

	//-----USL_UserWidget-----

	virtual void SetupDelegateToObject_Implementation(UObject* _ObjectIn) override;

	//-----IMinimapInterface-----
public:
	virtual void OnControlAreaAdded(AControlArea* _ControlArea) override;
	virtual void OnSoldierAddedToTeam(ASoldier* _Soldier) override {}
	virtual void OnSoldierRemovedFromTeam(ASoldier* _Soldier) override {}
	virtual void OnPingAdded(FVector2D _Ping) override {}
	virtual void OnUpdatePOIs() override {}
	virtual void OnFullMapDisplayBegin() override {}
	virtual void OnFullMapDisplayEnd() override {}
	virtual void OnPingDestroyed() override {}
};
