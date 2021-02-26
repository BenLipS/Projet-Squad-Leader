// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBase.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
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

protected:
	TArray<UGridBase*> m_GridBases;

public:
	/*
	* The dimension of the Grid
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_X = 100;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_Y = 100;

	/*
	* The dimension of a GridBase
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_X = 10;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_Y = 10;

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
	void AddGridBase(int index_i, int index_j, FVector _location);
};
