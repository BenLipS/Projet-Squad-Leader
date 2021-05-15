// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Info.h"
#include "../../Soldiers/AIs/SoldierAI.h"
#include "../../Soldiers/Players/SoldierPlayer.h"
#include "PlayerParam.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API APlayerParam : public AInfo
{
	GENERATED_BODY()

public:
	APlayerParam();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Params")
	int TeamID = 0;  // 0 for null
	UPROPERTY(EditDefaultsOnly, Category = "Params")
	TSubclassOf<ASoldierPlayer> PlayerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Params")
	TArray<TSubclassOf<ASoldierAI>> AIClass;

	UPROPERTY(EditDefaultsOnly, Category = "Reference")
		TArray<TSubclassOf<ASoldierPlayer>> ReferenceSoldier;
	UPROPERTY(EditDefaultsOnly, Category = "Reference")
		TArray<TSubclassOf<ASoldierAI>> ReferenceClassAITeam1;
	UPROPERTY(EditDefaultsOnly, Category = "Reference")
		TArray<TSubclassOf<ASoldierAI>> ReferenceClassAITeam2;

public:
	const int GetTeam() { return TeamID; }
	void SetTeam(int Id) { TeamID = Id; }

	TSubclassOf<ASoldierPlayer> GetPlayerClass() { return PlayerClass; }
	void SetPlayerClass(TSubclassOf<ASoldierPlayer> NewClass) { PlayerClass = NewClass; }

	TSubclassOf<ASoldierAI> GetAIClass(int AINb);
	void SetAIClass(TSubclassOf<ASoldierAI> NewClass, int AINb);


public:
	void AdaptAllAIToTeam();
	void AdaptAIToTeam(int AINb);
};
