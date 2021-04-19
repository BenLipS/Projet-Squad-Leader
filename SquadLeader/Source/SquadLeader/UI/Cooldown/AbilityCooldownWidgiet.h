// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "../Interface/AbilityCooldownInterface.h"

#include "SquadLeader/SquadLeader.h"

#include "Components/HorizontalBox.h"

#include "AbilityCooldownWidgiet.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UAbilityCooldownWidgiet : public USL_UserWidget, public IAbilityCooldownInterface
{
	GENERATED_BODY()

protected:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn) override;
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* CooldownBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCooldownElementWidget> WidgetElementClass;

public:
	virtual void OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key) override;

	virtual void AddAbilityID(ESoldierAbilityInputID Key, FString KeyText) override;
};