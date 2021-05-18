// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuListInfo.h"

#include "SquadLeader/Soldiers/Soldier.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Slate/SlateBrushAsset.h"

#include "MenuListClass.generated.h"

class UMenuListClassItemWidget;

UCLASS()
class SQUADLEADER_API UMenuListClass : public UMenuListInfo
{
	friend class UMenuListClassItemWidget;
	GENERATED_BODY()

//-----UMenuItemWidget-----
protected:
	//virtual void OnItemAddedToLayout_Implementation() override;
//-----UMenuItemWidget-----

	virtual void SynchronizeProperties() override;
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ClassButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	SoldierClass CurrentClass = SoldierClass::NONE;

	UPROPERTY(EditAnywhere)
	TMap<SoldierClass, FSoldierIconAsset> ClassAsset;

	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;

protected:

	UFUNCTION()
	FSoldierIconAsset GetAssetFromClass(SoldierClass SoldierClassIn);
	
	UFUNCTION()
	void SetAssetFromClass(SoldierClass SoldierClassIn);

	virtual void OnItemSelected() override;
	virtual void OnItemDeselected() override;

	//Button
	UFUNCTION()
	void OnClassButtonPressed();

public:
	UFUNCTION(BlueprintCallable)
	SoldierClass GetCurrentClass();
};
