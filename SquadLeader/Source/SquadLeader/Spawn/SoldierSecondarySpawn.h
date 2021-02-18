// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "SoldierPrimarySpawn.h"
#include "../Soldiers/SoldierTeam.h"
#include "Net/UnrealNetwork.h"
#include "SoldierSecondarySpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierSecondarySpawn : public ASoldierPrimarySpawn
{
	GENERATED_BODY()

public:
	ASoldierSecondarySpawn();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnCondition")
		bool initialyCanBeUsed;
};
