// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoldierSpawn.h"
#include "SoldierSecondarySpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierSecondarySpawn : public ASoldierSpawn
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
};
