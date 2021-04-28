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
	None UMETA(DisplayName = "None"),
};

USTRUCT()
struct SQUADLEADER_API FGridPackage {

	GENERATED_USTRUCT_BODY()
		FGridPackage() {}

	int team_value = 0;
	FVector m_location_on_map;
	TEnumAsByte<Type> m_type;
	uint32 ActorID;
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

	//value of the tile
	float m_value = 0.f;

	//the position of the tile in the world
	FVector m_location;
	
	//List of all the team that are on the tile
	//This will help to determine if there's a conflict on a certain position
	TArray<uint32> Teams;

	//The list of the different type on the tile(Soldier, COntrolArea, Projectile, etc.)
	TArray<TEnumAsByte<Type>> Types;

	//ID of the actor's that have influence on the tile
	TArray<uint32> ActorsID;

	bool in_update = false;

public:

	void Reset();

	void ReduceInfluence();
};


USTRUCT()
struct SQUADLEADER_API FActorData {
	GENERATED_USTRUCT_BODY()
public:
	FActorData(){}
public:
	uint32 ActorID;
	TArray<uint32> IndexInfluence;

public:

	void AddIndexs(const uint32 index) noexcept { IndexInfluence.Add(index); }
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

public:
	float GetValue(const FVector2D Location, const uint8 Team);
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
	bool FindTileIndex(const FVector _location, uint32& Index) const noexcept;

	bool FindIndexModifyinTeam1(const FVector2D Location, uint32& Index);
	bool FindIndexModifyinTeam2(const FVector2D Location, uint32& Index);

	/*
	* Check if _location is on the tile who's in tile_location
	*/
	bool IsOnTile(FVector _location, FVector tile_location) const noexcept;

	bool IsOnTileUpdate(const FVector2D Location, const FVector TileLocation) const noexcept;

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
	void InfluenceSoldier(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept;

	/*
	* Calculate the influence of a control area
	*/
	void InfluenceControlArea(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept;

	void InfluenceProjectile(int index, int start_index, int source_index, int distance, uint32 actorID, uint32 Team, uint16& ActorDataIndex) noexcept;

	/*
	* Calculate the time of execution of a function
	*/
	void TimeFunction();

	void UpdateTile(int index, float value, int team, Type type, uint32 actorID, uint16& ActorDataIndex) noexcept;

	void AddUpdateTileTeam1(const uint32 index);
	void AddUpdateTileTeam2(const uint32 index);

public:

	//Dimension of the grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		int m_grid_width = 100000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		int m_grid_height = 100000;

	//Dimension of a tile-base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
		int m_tile_width = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
		int m_tile_height = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
		float m_height = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		FVector m_startLocation = FVector{ 0.f,0.f,0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		TArray<float> m_heightList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool m_DrawAllGrid = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool m_DrawCharacterInfluence = false;

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
		TArray<uint32> m_index_update;

	UPROPERTY()
		TArray<uint32> m_index_team1;
	UPROPERTY()
		TArray<uint32> m_index_team2;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		float ProjectileInfluence = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		int ProjectileAreaInfluence = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		float ControlAreaInfluence = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		int ControlAreaAreaInfluence = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		float CharacterInfluence = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Influence Value")
		int CharacterAreaInfluence = 10;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Update Tick")
		int TickUpdate = 5;

	UPROPERTY()
		int CurrentTick = 5;

protected:

	UPROPERTY()
		TArray<FActorData> ActorsData;

	bool ActorAlreadyExist(const uint32 ActorID, uint16 &Index) const;

	void DeleteInfluence(const uint16 IndexActor, const uint8 Team) noexcept;

	void UpdateControlArea(const uint16 IndexControlArea, const uint8 Team) noexcept;
	void UpdateSoldier(const uint16 IndexSoldier, const uint8 Team, const uint32 SoldierID) noexcept;
};
