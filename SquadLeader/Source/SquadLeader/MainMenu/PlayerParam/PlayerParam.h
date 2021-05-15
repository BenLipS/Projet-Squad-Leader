// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Info.h"
#include "../../Soldiers/Soldier.h"
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
	SoldierClass PlayerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Params")
	TArray<SoldierClass> AIClass;

	UPROPERTY(EditDefaultsOnly, Category = "Reference")
	TMap<SoldierClass, TSubclassOf<ASoldierPlayer>> ReferenceSoldier;
	UPROPERTY(EditDefaultsOnly, Category = "Reference")
	TMap<SoldierClass, TSubclassOf<ASoldierAI>> ReferenceClassAITeam1;
	UPROPERTY(EditDefaultsOnly, Category = "Reference")
	TMap<SoldierClass, TSubclassOf<ASoldierAI>> ReferenceClassAITeam2;

public:
	const int GetTeam() { return TeamID; }
	void SetTeam(int Id) { TeamID = Id; }

	const SoldierClass GetPlayerClass() { return PlayerClass; }
	void SetPlayerClass(SoldierClass NewClass) { PlayerClass = NewClass; }
	const TSubclassOf<ASoldierPlayer> GetPlayerSoldier();

	const SoldierClass GetAIClass(int AINb);
	const TArray<SoldierClass> GetAllAIClass() { return AIClass; };
	void SetAIClass(SoldierClass NewClass, int AINb);
	const TSubclassOf<ASoldierAI> GetAISoldier(int AINb);
};
