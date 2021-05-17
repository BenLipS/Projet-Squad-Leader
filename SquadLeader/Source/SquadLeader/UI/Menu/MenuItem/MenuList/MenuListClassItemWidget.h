// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//CORE
#include "CoreMinimal.h"

//PARENT
#include "MenuListInfoItemWidget.h"

//INTERFACE
#include "SquadLeader/Soldiers/Soldier.h"

//UNREAL
#include "Components/Image.h"
#include "Components/Button.h"

//GENERATED
#include "MenuListClassItemWidget.generated.h"

class UMenuListClass;

UCLASS()
class SQUADLEADER_API UMenuListClassItemWidget : public UMenuListInfoItemWidget
{
	friend class UMenuListClass;
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	SoldierClass CurrentClass = SoldierClass::NONE;

	UPROPERTY()
	FSoldierIconAsset ClassAsset;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ClassButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IconImage;

protected:

	UFUNCTION()
	void SetPlayerClass(SoldierClass newClass);

//-----UMenuListInfoItemWidget-----
protected:
	virtual void OnItemSelect() override;
	virtual void OnItemDeselect() override;
};
