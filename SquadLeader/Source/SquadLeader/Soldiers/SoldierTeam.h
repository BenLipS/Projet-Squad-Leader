// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
// forward declaration
class ASoldier;
class ASoldierSpawn;

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../Interface/PreInitable.h"
//#include "Soldier.h"
//#include "../ControlArea/ControlArea.h"
#include "SoldierTeam.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API ASoldierTeam : public AInfo, public IPreInitable
{
	GENERATED_BODY()

public:
	ASoldierTeam();

	// Pre init launch by the gameMode before the BeginPlay() function
	virtual void PreInitialisation() override;
	virtual int getpriority() override;

protected:
	virtual void BeginPlay() override;

public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "GlobalTeamData")
		FString TeamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GlobalTeamData")
		int Id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "GlobalTeamData")
		int NbAIBasic = 6;


public:  // Soldier List
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "SoldierList")
		TArray<ASoldier*> soldierList;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> ClassBasicAI;

public:
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> GetClassBasicAI();

	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void AddSoldierList(ASoldier* newSoldier);
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void RemoveSoldierList(ASoldier* newSoldier);
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void CleanSoldierList();


protected: // Spawn points
	UPROPERTY(VisibleAnywhere, Replicated, Category = "SpawnPoints")
		TArray<ASoldierSpawn*> mainSpawnPoints;
public:
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void AddSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void RemoveSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void CleanSpawnPoints();
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		TArray<ASoldierSpawn*> GetUsableSpawnPoints();


protected:  // Tickets
	UPROPERTY(EditAnywhere, Replicated, Category = "Tickets")
		int Tickets = 5;  // default value, must be changed in blueprint
public:
	UFUNCTION(BlueprintCallable, Category = "Tickets")
		void RemoveOneTicket();
	UFUNCTION(BlueprintCallable, Category = "Tickets")
		int GetTicket() const { return Tickets; }
};
