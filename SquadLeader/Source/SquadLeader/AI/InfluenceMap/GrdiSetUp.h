// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBase.h"
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

	void DrawGrid();

	void CreateGrid();

protected:
	TArray<UGridBase*> m_GridBases;

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_X = 100;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridSize_Y = 100;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_X = 10;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Grid")
		int m_GridBaseSize_Y = 10;

	UPROPERTY()
		int size_array; 
	
	UPROPERTY()
		int size_array_X;

	UPROPERTY()
		int size_array_Y;
	
};
