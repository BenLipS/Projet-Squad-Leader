// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_Projectile.h"
#include "Projectile_Mine.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API AProjectile_Mine : public ASL_Projectile
{
	GENERATED_BODY()

public:
	AProjectile_Mine();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitVelocity();

	UPROPERTY()
	bool Placed = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile|Mine")
	float DetectionRadius = 100.f;

public:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnEndOfDelay() override;
		
	virtual void OnStick() override;
};