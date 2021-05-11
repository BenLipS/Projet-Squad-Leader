// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../MenuItemWidget.h"

#include "Components/PanelWidget.h"
#include "Components/NamedSlot.h"

#include "SquadLeader/MainMenu/GameParam/GameParam.h"

#include "MenuCollectionDataInt.h"
#include "MenuCollectionDataBool.h"
#include "MenuCollectionDataText.h"

#include "MenuDataCollectionWidget.generated.h"

/**
 * 
 */

class UMenuCollectionDataItem;
UCLASS()
class SQUADLEADER_API UMenuDataCollectionWidget : public UMenuItemWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNamedSlot* DataSlot;

	UPanelWidget* DataContainer;

	TMap<FString, UMenuCollectionDataInt*> DataInts;

	TMap<FString, UMenuCollectionDataText*> DataStrings;

	TMap<FString, UMenuCollectionDataBool*> DataBools;

	virtual void SynchronizeProperties() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGameParam> DefaultValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGameParam> MinValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGameParam> MaxValues;

//-----UMenuItemWidget-----
protected:
	virtual void OnItemAddedToLayout_Implementation();
//-----UMenuItemWidget-----

protected:
	UFUNCTION(BlueprintCallable)
	void InitIntValues();

	UFUNCTION(BlueprintCallable)
	void RandomizeIntValues();

/*public:
	UFUNCTION(BlueprintCallable)
	int GetIntData(FString Key);*/
};