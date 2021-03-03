// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBase.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "../../SquadLeaderGameModeBase.h"
#include "GrdiSetUp.generated.h"

UCLASS()
class SQUADLEADER_API AGrdiSetUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrdiSetUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		int FindTile(FVector _location);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	* Print the grid 
	* Make the FPS fall
	* Only use this for debbug
	*/
	void DrawGrid();
	
	/*
	* Create and generate the grid that will reprsent
	* our map
	*/
	void CreateGrid();

	/*
	* Return the value of the influence 
	* at the _location
	*/
	float GetValue(FVector _location);

protected:
	TArray<float> m_GridBases;

	ASquadLeaderGameModeBase* m_Gamemode;
public:
	/*
	* The dimension of the Grid
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_X = 100000;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_Y = 100000;

	/*
	* The dimension of a GridBase
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_X = 200;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_Y = 200;


private:
	/*
	* The size of the array m_GridBases
	*/
	UPROPERTY()
		int size_array; 
	
	/*
	* the dimension of the array
	*/
	UPROPERTY()
		int size_array_X;

	UPROPERTY()
		int size_array_Y;

	UPROPERTY()
		int m_nbr_tick = 0.f;
private:

	/*
	* Check if the Location is a valid Location in the world
	* navSys is here because we will need to see if the _location 
	* is reecheable
	*/
	bool IsValidLocation(FVector _location, UNavigationSystemV1* navSys);

	/*
	* Add a GridBase to the array
	* if _location is empty then it put a NULL
	* pointer in the array.
	*/
	void AddGridBase(int index_i, int index_j, bool _isvalid);

	/*
	* we clean the grid and reset it
	*/
	void CleanGrid();

	/*
	* we update the grid with date comming from the environment
	*/
	void UpdateGrid();

	/*
	*Update all the info of the player in the influence map 
	*/
	void UpdateGridSoldier();

	/*
	* Update all the info of the control area in the map
	*/
	void UpdateGridControlArea();

	/*
	* Set The value of 
	* a Grid Base who's index is _index
	*/
	void SetValue(int _index, float _value);


	/*
	* Set Up all the good tile in the radius of influence of the character
	*/
	void SetRadiusValue(int _index, float _value, int _radius);
};
