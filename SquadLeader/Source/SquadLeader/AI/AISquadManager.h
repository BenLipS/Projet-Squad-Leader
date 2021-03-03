// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Soldiers/Players/SoldierPlayer.h"
#include "AISquadController.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "Mission.h"
#include "AISquadManager.generated.h"


/**
 * 
 */

UENUM()
enum class FormationType { Circle, Arrow };

UCLASS(Blueprintable)
class SQUADLEADER_API AAISquadManager : public AInfo
{
	GENERATED_BODY()

	UPROPERTY()
	UMission* Mission;
public:
	AAISquadManager();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ASoldierAI> ClassAI;

	UFUNCTION()
	void Init(TSubclassOf<ASoldierTeam> _Team, ASoldierPlayer* _Player);
	
	UPROPERTY()
	TSubclassOf<ASoldierTeam> Team;

	UPROPERTY()
	ASoldierPlayer* Leader;

	UPROPERTY()
	TArray<AAISquadController*> AISquadList;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	UMission* GetMission() { return Mission; };

	//For Formation
	TArray<FVector> FormationPos;

	FormationType TypeOfFormation;

	UFUNCTION()
	void UpdateFormation();

	UFUNCTION()
	void UpdateCircleFormation();


};
