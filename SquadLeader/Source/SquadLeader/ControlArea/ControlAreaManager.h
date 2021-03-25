// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../Interface/PreInitable.h"
#include "ControlArea.h"
#include "../Soldiers/SoldierTeam.h"
#include "ControlAreaManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AControlAreaManager : public AInfo, public IPreInitable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AControlAreaManager();
protected:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "ControlList")
		TArray <AControlArea*> ControlAreaList;  // private because it must not be edited elsewhere

public:
	// Pre init launch by the gameMode before the BeginPlay() function
	virtual void PreInitialisation() override;
	virtual int getpriority() override;
	
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
	TArray <AControlArea*> GetAreaControlledByTeam(ASoldierTeam* _Team);

	UFUNCTION()
	ASoldierTeam* GetTeamWithMostControl();
	
	UFUNCTION()
	ASoldierTeam* GetTeamWithAllControl();
};
