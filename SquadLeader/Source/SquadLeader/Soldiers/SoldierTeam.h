// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "SoldierTeam.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierTeam : public AInfo
{
	GENERATED_BODY()

public:
	ASoldierTeam();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, Replicated, Category = "TeamData")
		FString TeamName;
	
};
