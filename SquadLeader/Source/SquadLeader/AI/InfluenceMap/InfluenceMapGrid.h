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
	bool in_update = false;
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
	void InfluenceSoldier(int index, int start_index, int source_index, int distance) noexcept;

	/*
	* Calculate the influence of a control area
	*/
	void InfluenceControlArea(int index, int start_index, int source_index, int distance, int value) noexcept;

	/*
	* Calculate the time of execution of a function
	*/
	void TimeFunction();

	void UpdateTile(int index, float value, int team, Type type) noexcept;

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

	/*
	* Will contains the index of the tile that need
	* to be update
	* with that we don't need tu update all the grid
	* we pop out an index when he's update value is down to 0
	*/
	UPROPERTY()
		TArray<int> m_index_update;

	int value_tick = 0;
	/*class UMyThreadManager* m_ThreadManager;*/
};
