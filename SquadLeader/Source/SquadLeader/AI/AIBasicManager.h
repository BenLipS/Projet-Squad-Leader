// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIBasicController.h"
#include "../ControlArea/ControlAreaManager.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "AIBasicManager.generated.h"


UENUM()
enum class AIAvaibility : uint8 {
	available, unavailable
};

USTRUCT()
struct SQUADLEADER_API FIndexSoldier {
	GENERATED_USTRUCT_BODY()
		FIndexSoldier() = default;

	TArray<uint32> SoldierIndex;

};


/**
 * 
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AAIBasicManager : public AInfo
{
	GENERATED_BODY()

public:

	using ai_type = TSharedPtr<AAIBasicController>;

	AAIBasicManager();

	UFUNCTION()
	void Init(ASoldierTeam* _Team);

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;


	UPROPERTY()
		TArray<AAIBasicController*> AIBasicList;

	UPROPERTY()
		TArray<uint32> AIBasicAvailable;

	UPROPERTY()
		TArray<uint32> AIBasicUnavailable;

	UPROPERTY()
	ASoldierTeam* Team;

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
		void ChooseControlArea();

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		TSubclassOf<UNavigationQueryFilter> m_queryFilter_team1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		TSubclassOf<UNavigationQueryFilter> m_queryFilter_team2;


public:
	UFUNCTION()
		void ChangeAIStatus(const AIAvaibility status, const uint32 IndexSoldier);

protected:
	void AIAvailable(const uint32 IndexSoldier);
	void AIUnavailable(const uint32 IndexSoldier);

protected:
	//Map d'index, pour des index
	//La cle sera l'index de la control area, cette index permet de retrouver la controle area dans la Liste de controle area du ControlAreaManager
	//La valeur est une liste d'index pour les soldat pour les retrouver dans la liste de AIBasic.
	UPROPERTY()
		TMap<uint32, FIndexSoldier> ListSoldierOnControlArea;

	void InitListSoldierOnControlArea();


protected:
	UPROPERTY()
		TArray<uint32> ControlAreaAllies;
	UPROPERTY()
		TArray<uint32> ControlAreaEnnemies;
	UPROPERTY()
		TArray<uint32> ControlAreaNeutral;

public:
	UFUNCTION()
		void UpdateControlArea(const uint8 TeamID, const uint8 IndexControlArea);

protected:
	UPROPERTY()
		bool ControlAreasBeenUpdate = false;

	UPROPERTY()
		bool NewSoldierAvailable = false;

	UFUNCTION()
		void Strategy();

	UFUNCTION()
		bool FindControlAreaOn(const uint8 IndexSoldier, uint32& IndexControlArea);

	UFUNCTION()
		bool FindAvailableSoldier(uint32& IndexSoldier);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time per Frame")
		float TimeInterval = 0.02;

protected:
	UPROPERTY()
		int LastIndex = 0;
};
