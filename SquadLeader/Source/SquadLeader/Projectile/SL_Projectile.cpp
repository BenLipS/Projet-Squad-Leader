#include "SL_Projectile.h"
#include "Components/SPhereComponent.h"
#include "../AreaEffect/AreaEffect.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/AIs/SoldierAI.h"
#include "../AI/AIClassesController/AssaultAIBasicController.h"
#include "../AI/AIClassesController/AssaultAISquadController.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASL_Projectile::ASL_Projectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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

	if (OnContactPolicy == ContactPolicy::STICKY) {
		//CollisionComp->SetCollisionResponseToAllChannel(ECC_EngineTraceChannel1, ECR_Overlap);
		//CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	}
	CollisionComp->InitSphereRadius(Radius); //physic collision radius

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASL_Projectile::OnOverlapBegin);

	if (OnContactPolicy == ContactPolicy::EXPLODE || OnContactPolicy == ContactPolicy::STICKY)
		CollisionComp->OnComponentHit.AddDynamic(this, &ASL_Projectile::OnHit);

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = OnContactPolicy == ContactPolicy::BOUNCE;
	ProjectileMovement->Bounciness = Bounciness;
	ProjectileMovement->ProjectileGravityScale = GravityScale;

	InitVelocity();

	//if (auto temp = Cast<USphereComponent>(RootComponent))
	//	temp->IgnoreActorWhenMoving(GetOwner(), true);

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

void ASL_Projectile::OnStick()
{
}

void ASL_Projectile::DeleteProjectile()
{
	Destroy();
}

void ASL_Projectile::InitVelocity()
{
	FVector ForwardVector;
	ASoldier* soldier = Cast<ASoldier>(GetOwner());

	if (soldier) {
		ForwardVector = soldier->GetLookingDirection();
	}
	else if (GetOwner()) {
		ForwardVector = GetOwner()->GetActorForwardVector();
	}
	//Fixing radius to 1
	ForwardVector.Normalize();

	float Azimuth = UKismetMathLibrary::DegAcos(ForwardVector.Z);
	float Inclination = UKismetMathLibrary::DegAtan2(ForwardVector.Y, ForwardVector.X);

	if (ASoldierAI* AISoldier = Cast<ASoldierAI>(GetOwner()); AISoldier) {
		if (AAIGeneralController* AI = Cast<AAIGeneralController>(Cast<ASoldierAI>(GetOwner())->Controller); AI) {
			PitchAdjust = AI->LaunchProjectilePitchAdjust;
			YawAdjust = AI->LaunchProjectileYawAdjust;
		}
	}	


	Azimuth += PitchAdjust;
	if (Azimuth > 180.f)
	{
		Azimuth = 180.f;
	}
	else if (Azimuth < 0.f)
	{
		Azimuth = 0.f;
	}

	Inclination += YawAdjust;
	Inclination = UKismetMathLibrary::GenericPercent_FloatFloat(Inclination, 360.f);

	ForwardVector = FVector(UKismetMathLibrary::DegSin(Azimuth) * UKismetMathLibrary::DegCos(Inclination), UKismetMathLibrary::DegSin(Azimuth) * UKismetMathLibrary::DegSin(Inclination), UKismetMathLibrary::DegCos(Azimuth));

	ProjectileMovement->Velocity =  ForwardVector * ProjectileMovement->InitialSpeed;
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
			/*CollisionComp->Deactivate();
			ProjectileMovement->Deactivate();*/
			AttachToActor(OtherActor, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			OnStick();
			//AttachToActor(OtherActor, FAttachmentTransformRules::KeepRelativeTransform);
			//AttachToComponent(OtherComponent, FAttachmentTransformRules::KeepRelativeTransform);
			break;
		default:
			break;
		}
	}
	
}

void ASL_Projectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OnContactPolicy == ContactPolicy::EXPLODE)
	{
		ASoldier* soldierOwner = Cast<ASoldier>(GetOwner());
		ASoldier* soldier = Cast<ASoldier>(OtherActor);

		if (soldier)
		{
			if (soldierOwner)
			{
				if (soldierOwner->GetTeam() != soldier->GetTeam())
				{
					OnExplode();
				}
			}
			else
			{
				OnExplode();
			}
		}
		else
		{
			OnExplode();
		}
	}
	else if (OnContactPolicy == ContactPolicy::STICKY)
	{
		ASoldier* soldier = Cast<ASoldier>(OtherActor);
		if (soldier)
		{
		}
		else
		{
			//ProjectileMovement->Deactivate();
			AttachToActor(OtherActor, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			OnStick();
		}
	}
}


//FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());