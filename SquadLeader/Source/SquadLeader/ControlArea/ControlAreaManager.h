// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ControlArea.h"
#include "../Soldiers/SoldierTeam.h"
#include "ControlAreaManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AControlAreaManager : public AInfo
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AControlAreaManager();
	
public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
		void AddControlArea(AControlArea* _NewArea);
	UFUNCTION()
		TArray <AControlArea*> GetControlArea() { return ControlAreaList; }
	UFUNCTION()
	void CleanControlAreaList();

	UFUNCTION()
	TArray <AControlArea*> GetAreaControlledByTeam(TSubclassOf<ASoldierTeam> _Team);

	UFUNCTION()
	TSubclassOf<ASoldierTeam> GetTeamWithMostControl();

protected:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "ControlList")
	TArray <AControlArea*> ControlAreaList;  // private because it must not be edited elsewhere
};
