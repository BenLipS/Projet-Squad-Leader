#include "Projectile_Mine.h"
#include "Components/SPhereComponent.h"
#include "../AreaEffect/AreaEffect.h"
#include "../Soldiers/Soldier.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"

AProjectile_Mine::AProjectile_Mine() : ASL_Projectile()
{
	OnContactPolicy = ContactPolicy::STICKY;
}

void AProjectile_Mine::BeginPlay()
{
	Super::BeginPlay();
	auto GM = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM && GM->InfluenceMap) {
		FGridPackage Package;
		Package.m_location_on_map = GetActorLocation();
		Package.team_value = 1;
		Package.m_type = Type::Projectile;
		Package.ActorID = this->GetUniqueID();
		GM->InfluenceMap->ReceivedMessage(Package);
	}
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
	if (Placed)
	{
		if (ASoldier* TargetSoldier = Cast<ASoldier>(OtherActor); TargetSoldier)
		{
			if (ASoldier* SourceSoldier = Cast<ASoldier>(GetInstigator()); SourceSoldier && SourceSoldier->GetTeam() != TargetSoldier->GetTeam())
				OnExplode();
		}
	}
	else
		Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AProjectile_Mine::OnExplode()
{
	Super::OnExplode();
	GEngine->AddOnScreenDebugMessage(10, 1.f, FColor::Cyan, TEXT("Mine destructor"));
}

void AProjectile_Mine::OnStick()
{
	Super::OnStick();
	Placed = true;
	CollisionComp->SetSphereRadius(DetectionRadius, true);
	//CollisionComp->UpdateOverlaps();
}