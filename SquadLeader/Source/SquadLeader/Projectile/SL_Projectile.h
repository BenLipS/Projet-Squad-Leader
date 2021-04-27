#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_Projectile.generated.h"

class AAreaEffect;

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
	ASL_Projectile();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;

public:
	UStaticMeshComponent* GetMesh() const;

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
	float ExplosionDelay = 15.f; // 0.f pour infini

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float Bounciness = 0.3f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float YawAdjust;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	float PitchAdjust;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	ContactPolicy OnContactPolicy;

	// AreaEffect to apply on contact/explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	TArray<TSubclassOf<AAreaEffect>> AreaEffectList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* CollisionComp;

	FTimerHandle TimerExplosion;

	void DeleteProjectile();
	virtual void InitVelocity();

public:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void OnExplode();

	UFUNCTION()
	virtual void OnStick();

//////////////// Collision
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Collision")
	bool bDebugTraceExplosion = false;
};
