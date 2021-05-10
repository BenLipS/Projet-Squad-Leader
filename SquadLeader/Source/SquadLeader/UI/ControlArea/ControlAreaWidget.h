// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../SL_UserWidget.h"
#include "../Interface/MinimapInterface.h"
#include "../Interface/TicketInterface.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

#include "ControlAreaWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UControlAreaWidget : public USL_UserWidget, public IMinimapInterface, public ITicketInterface
{
	GENERATED_BODY()

protected:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* AreaContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* LeftTicketText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RightTicketText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* LeftGlow;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* RightGlow;

protected:
	TArray<AControlArea*> ControlAreaList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UControlAreaInfoWidget> ControlAreaInfoClass;

protected:
	virtual void SynchronizeProperties() override;


//-----ITicketInterface-----
public:
	virtual void OnAllyTicketChanged(int newTicket) override;
	virtual void OnEnnemyTicketChanged(int newTicket) override;

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
