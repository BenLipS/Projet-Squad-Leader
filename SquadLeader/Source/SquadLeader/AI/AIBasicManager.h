// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIBasicController.h"
#include "../ControlArea/ControlAreaManager.h"
#include "AIBasicManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AAIBasicManager : public AInfo
{
	GENERATED_BODY()

public:
	AAIBasicManager();

	UFUNCTION()
	void Init(TSubclassOf<ASoldierTeam> _Team);

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;


	UPROPERTY()
	TArray<AAIBasicController*> AIBasicList;

	UPROPERTY()
	TSubclassOf<ASoldierTeam> Team;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector InitSpawnDiameter = FVector{ 500.f, 0.f, 0.f };

private:
	// Calculate Initial position for initial spawn
	FVector CalculOffSetForInitSpawn(ASoldierSpawn* spawnpoint, int NbAIToSpawn, int AiNb);

	/*
	* Initialize the number of unit and the number of control area 
	*/
	UFUNCTION()
		void InitValue();

	/*
	* Will determine the first order of all the AI
	* Happend only once at the beginning
	*/
	UFUNCTION()
		void ChooseCOntrolArea();

private:

	/*
	* Get the number of control area in the world
	*/
	UPROPERTY()
		int nbr_controlArea;

	/*
	* Get the number of unit available 
	*/
	UPROPERTY()
		int nbr_unite;

	UPROPERTY()
		AControlAreaManager* m_controlAreaManager;

};
