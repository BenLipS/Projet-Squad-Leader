// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Info.h"
#include "InfluenceMapGrid.generated.h"

USTRUCT()
struct SQUADLEADER_API FNeighboor {

	GENERATED_USTRUCT_BODY()

	FNeighboor() {
	}

	TArray<int> m_neighboor;

};


UENUM()
enum Type {
	Soldier UMETA(DisplayName = "Soldier"),
	ControlArea UMETA(DisplayName = "ControlArea"),
	Projectile UMETA(DisplayName = "Projectile"),
};

USTRUCT()
struct SQUADLEADER_API FGridPackage {

	GENERATED_USTRUCT_BODY()
		FGridPackage() {}

	int team_value = 0;
	FVector m_location_on_map;
	TEnumAsByte<Type> m_type;
};

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
	TEnumAsByte<Type> m_type;
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

	void ReceivedMessage(FGridPackage _message);

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

	/*
	* Reset all the grid
	* the value of a tile is reset to 0
	* the team of a tile is reset to none
	*/
	void ResetGrid() noexcept;

	/*
	* Update the grid with world information
	*/
	void UpdateGrid() noexcept;

	/*
	* Return the Index in the grid of a certain _location
	*/
	int FindTileIndex(FVector _location) const noexcept;

	/*
	* Check if _location is on the tile who's in tile_location
	*/
	bool IsOnTile(FVector _location, FVector tile_location) const noexcept;

	/*
	* Find the neighboor for all tile
	*/
	void FindNeighboor() noexcept;

	/*
	* Find the neighboor for a specific tile
	*/
	void Neighboors(int index) noexcept;

	/*
	* Check if two tile are neighboor
	*/
	void IsNeighboor(int index_1, int index_2) noexcept;

	/*
	* Algorithm recursif
	* calculate the influence of player on the grid
	*/
	void Influence(int index, int start_index, int source_index, int distance) noexcept;


	/*
	* Update all the players for the grid
	*/
	void UpdatePlayers() noexcept;

	/*
	* Update the information of control area in the influence map
	*/
	void UpdateControlArea() noexcept;

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
	
	/*
	* List the neighboors of a tile
	*/
	UPROPERTY()
		TArray<FNeighboor> m_neighboors;


	int value_tick = 0;
	/*class UMyThreadManager* m_ThreadManager;*/
};
