// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBase.h"
#include "GridSetUp.generated.h"

UCLASS()
class SQUADLEADER_API AGridSetUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridSetUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	* Array of the GridBase that are going to be in the world
	*/
	TArray<AGridBase*> m_GridBases;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSetUp")
		int Grid_X = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSetUp")
		int Grid_Y = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSetUp")
		int Grid_Size = 128;

	UPROPERTY(EditDefaultsOnly, Category = "GridSetUp")
		TSubclassOf<class AGridBase> m_BP_AGridBase;

};
