// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Info.h"
#include "InfluenceMapGrid.generated.h"

/*
* This struct represent a Tile-Base for the grid
* it contains a value, a location and the information of witch team they are of the influence
*/
USTRUCT()
struct SQUADLEADER_API FTileBase {

	GENERATED_USTRUCT_BODY()

	FTileBase() {
	}

	float m_value = 0.f;
	FVector m_location;
	int m_team = -1;

};


/**
 * This class will create a Grid for the influence Map
 * It will also contains the Infleunce Map
 */
UCLASS(Blueprintable)
class SQUADLEADER_API AInfluenceMapGrid : public AInfo
{
	GENERATED_BODY()

public:
	AInfluenceMapGrid();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	/*
	* will initialize the array with the tile needed
	*/
	void InitGrid() noexcept;

	/*
	* will check if the location is good
	* we do that with the help of the nav-mesh
	*/
	bool IsValid(FVector _location) const;

	/*
	* Draw the grid
	* only use this for debug
	*/
	void DrawGrid() const;

	void ResetGrid() noexcept;

public:

	//Dimension of the grid
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_grid_width = 100000;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_grid_height = 100000;

	//Dimension of a tile-base
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_tile_width = 200;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_tile_height = 200;

private:

	/*
	* Array of the tile-base for the influence map
	*/
	UPROPERTY()
		TArray<FTileBase> m_influencemap;

	//The number of tile in the end
	int nbr_tile = 0;
	
};
