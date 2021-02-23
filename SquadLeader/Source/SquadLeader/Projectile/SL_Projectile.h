// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_Projectile.generated.h"

UENUM()
	enum class ContactPolicy {
	BOUNCE UMETA(DisplayName = "Bounce"),
	STICKY UMETA(DisplayName = "Sticky"),
	EXPLODE UMETA(DisplayName = "Explode")
};

UCLASS()
class SQUADLEADER_API ASL_Projectile : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ASL_Projectile();
	ASL_Projectile(FVector& FireDirection);

	~ASL_Projectile() = default;

	//Attributes
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float InitialSpeed = 1000.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float MaxSpeed = 100000.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float GravityScale = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float Radius = 10.f;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float LifeSpan = 10.f; // 0.f pour infini

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float Bounciness = 0.3f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	ContactPolicy OnContactPolicy = ContactPolicy::BOUNCE;

	// AreaEffect to apply on contact/explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	TArray<TSubclassOf<class AAreaEffect>> ExplosionAreaEffect;


	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* CollisionComp;

	FTimerHandle TimerExplosion;

	//Methods

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnExplode();

	void DeleteProjectile();

	void InitVelocity();

	void InitVelocity(FVector& FireDirection);

public:
	void OnContact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
