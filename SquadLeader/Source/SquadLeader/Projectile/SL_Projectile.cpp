// Fill out your copyright notice in the Description page of Project Settings.


#include "SL_Projectile.h"
#include "Components/SPhereComponent.h"
#include "../AreaEffect/AreaEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASL_Projectile::ASL_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->bShouldCollideWhenPlacing = false;

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
}

// Called when the game starts or when spawned
void ASL_Projectile::BeginPlay()
{
	Super::BeginPlay();


	CollisionComp->InitSphereRadius(Radius); //physic collision radius

	if (OnContactPolicy == ContactPolicy::EXPLODE || OnContactPolicy == ContactPolicy::STICKY)
		CollisionComp->OnComponentHit.AddDynamic(this, &ASL_Projectile::OnHit);

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = OnContactPolicy == ContactPolicy::BOUNCE;
	ProjectileMovement->Bounciness = Bounciness;
	ProjectileMovement->ProjectileGravityScale = GravityScale;


	InitVelocity();

	if (auto temp = Cast<USphereComponent>(RootComponent))
		temp->IgnoreActorWhenMoving(GetOwner(), true);

	if (ExplosionDelay > 0.0f)
		GetWorldTimerManager().SetTimer(TimerExplosion, this, &ASL_Projectile::OnExplode, ExplosionDelay, true);
}

void ASL_Projectile::OnExplode()
{
	for (auto areaEffect : ExplosionAreaEffect)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = GetOwner();
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AAreaEffect>(areaEffect, GetActorLocation(), GetActorRotation(), SpawnInfo);
	}
	DeleteProjectile();
}

void ASL_Projectile::DeleteProjectile()
{
	Destroy();
}

void ASL_Projectile::InitVelocity()
{
	if (GetOwner()) {
		ProjectileMovement->Velocity = GetOwner()->GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}
}

void ASL_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{

	}
	else
	{
		switch (OnContactPolicy) {
		case ContactPolicy::EXPLODE:
			OnExplode();
			break;
		case ContactPolicy::STICKY:
			break;
		default:
			break;
		}
	}
	
}
//FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());