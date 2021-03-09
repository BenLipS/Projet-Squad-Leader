// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Mine.h"
#include "Components/SPhereComponent.h"
#include "../AreaEffect/AreaEffect.h"
#include "../Soldiers/Soldier.h"

AProjectile_Mine::AProjectile_Mine() : ASL_Projectile()
{
	OnContactPolicy = ContactPolicy::STICKY;
}

void AProjectile_Mine::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile_Mine::InitVelocity()
{
	Super::InitVelocity();

}

void AProjectile_Mine::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Placed) {

	}
	else
	{
		Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
	}
}

void AProjectile_Mine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Placed) {
		ASoldier* soldier = Cast<ASoldier>(OtherActor);
		if (soldier)
		{
			OnExplode();
		}
	}
	else
	{
		Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void AProjectile_Mine::OnExplode()
{
	Super::OnExplode();
}

void AProjectile_Mine::OnStick()
{
	Super::OnStick();
	Placed = true;
	CollisionComp->SetSphereRadius(DetectionRadius, true);
	//CollisionComp->UpdateOverlaps();
}