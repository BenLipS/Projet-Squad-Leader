#pragma once

#include "Core.h"
#include "Engine/TargetPoint.h"
#include "Net/UnrealNetwork.h"
#include "SoldierSpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierSpawn : public ATargetPoint
{
	GENERATED_BODY()

public:
	ASoldierSpawn();

protected:
	virtual void BeginPlay() override;

public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditInstanceOnly, Replicated, Category = "SpawnCondition")
	bool canBeUsed;
	int RespawnLoop = 0;

public:
	UFUNCTION()
	bool GetUsable() { return canBeUsed; }

	UPROPERTY(EditInstanceOnly, Category = "SpawnData")
	int SpawnRadius = 200;
	UPROPERTY(EditInstanceOnly, Category = "SpawnData")
	int NbSpawnPosition = 6;

	FVector GetSpawnLocation();
};
