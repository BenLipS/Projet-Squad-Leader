// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MenuLayoutWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "SquadLeader/Soldiers/Soldier.h"

#include "ClassSelectionMenuLayout.generated.h"

/**
 * 
 */
class UMenuListClass;

UCLASS()
class SQUADLEADER_API UClassSelectionMenuLayout : public UMenuLayoutWidget
{
	GENERATED_BODY()
protected:
	int NbAI = 0;

	UPROPERTY(BlueprintReadOnly)
	UScrollBox* ClassBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UTextBlock> DescriptionClass;

	UPROPERTY(EditAnywhere, meta = (UIMin = "1", UIMax = "10"))
	int NbClassPreview = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMenuListClass> ClassItem;

	UPROPERTY(BlueprintReadOnly)
	TArray<UMenuListClass*> ListAIClass;

	UPROPERTY(BlueprintReadOnly)
	UMenuListClass* PlayerClass;

protected:
	virtual void SynchronizeProperties() override;

	virtual void SynchronizeAllMenuItem() override;

private:
	void SynchronizeAllMenuItem(UPanelWidget* PanelIn);

public:
	UMenuListClass* AddClassEntryToClassBox();

	UFUNCTION(BlueprintCallable)
	TArray<SoldierClass> GetAIClasslist();

	UFUNCTION(BlueprintCallable)
	SoldierClass GetPlayerClass();
};