// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockingComponent.h"
#include "../AIGeneralController.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationPath.h"

// Sets default values for this component's properties
UFlockingComponent::UFlockingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFlockingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFlockingComponent::ResetVectors()
{
	SeenBoids.Empty();
	PreviousMovementVector = MovementVector;
	AlignementVector = FVector::ZeroVector;
	CohesionVector = FVector::ZeroVector;
	SeparationVector = FVector::ZeroVector;
	ObjectifVector = FVector::ZeroVector;
	WallAvoidanceVector = FVector::ZeroVector;
	ShootingPositionVector = FVector::ZeroVector;
}

void UFlockingComponent::UpdateNeighbourhood()
{
	for (ASoldier* soldier : Cast<AAIGeneralController>(GetOwner())->GetSeenSoldier())
	{
		if (AAIGeneralController* AI = Cast<AAIGeneralController>(soldier->Controller); AI && soldier->GetTeam() == Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetTeam() /*&& Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation() == AI->GetObjectifLocation()*/)
			SeenBoids.Add(AI);
	};
}

void UFlockingComponent::UpdateCohesionVector()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	for (AAIGeneralController* Boid : SeenBoids)
	{
		if (Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation() == Boid->GetObjectifLocation()) {
			/* Cohesion throught pathfinding and not absolute*/
			UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), SoldierLocation, Boid->GetPawn()->GetActorLocation(), NULL);

			FVector CohesionLocalDir;
			if (path->PathPoints.Num() > 2)
				CohesionLocalDir = path->PathPoints[1];
			else
				CohesionLocalDir = Boid->GetPawn()->GetActorLocation();

			//DrawDebugPoint(GetWorld(), CohesionLocalDir, 10, FColor::Red);

			CohesionVector += CohesionLocalDir.GetSafeNormal(DefaultNormalizeVectorTolerance) * Boid->GetPawn()->GetActorLocation().Size() - SoldierLocation;
		}
	}

	CohesionVector = (CohesionVector / SeenBoids.Num()) / 100;
}

void UFlockingComponent::UpdateAlignementVector()
{
	for (AAIGeneralController* Boid : SeenBoids)
	{
		if (Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation() == Boid->GetObjectifLocation()) {
			if (AlignementVector.Size() > 0)AlignementVector += Boid->FlockingComponent->GetMovementVector().GetSafeNormal(DefaultNormalizeVectorTolerance);
		}
	}

	AlignementVector.Z = 0;//Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation().Z;

	if ((MovementVector + AlignementVector).Size() > 0)AlignementVector = (MovementVector + AlignementVector).GetSafeNormal(DefaultNormalizeVectorTolerance);
}

void UFlockingComponent::UpdateSeparationVector()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	
	for (ASoldier* SeenSoldier : Cast<AAIGeneralController>(GetOwner())->GetSeenSoldier())
	{
		//if ((SoldierLocation - SeenSoldier->GetActorLocation()).Size() < SeparationRange) {
		FVector Separation = SoldierLocation - SeenSoldier->GetActorLocation();
		SeparationVector += Separation.GetSafeNormal(DefaultNormalizeVectorTolerance) / FMath::Abs(Separation.Size() - BoidPhysicalRadius);
		//}
	}

	const FVector SeparationForceComponent = SeparationVector * 100;
	SeparationVector += SeparationForceComponent + SeparationForceComponent * (5 / SeenBoids.Num());
}

void UFlockingComponent::UpdateObjectifVector()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), SoldierLocation, Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation(), NULL);

	FVector ObjectifLocalDir;
	if (path) {
		if (path->PathPoints.Num() > 1)
			ObjectifLocalDir = path->PathPoints[1];
		else
			ObjectifLocalDir = Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation();
	}


	ObjectifLocalDir.Z = SoldierLocation.Z;

	//DrawDebugPoint(GetWorld(), ObjectifLocalDir, 10, FColor::Red);

	ObjectifVector = ObjectifLocalDir - SoldierLocation;
	ObjectifVector = ObjectifVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
}

void UFlockingComponent::UpdateShootingPositionVector()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	FVector EnemyLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsObject("FocusActor"))->GetLocation();
	FVector IdealShootingPosition = Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsVector("ShootingPosition");

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector HitLocation{};

	if (navSys->NavigationRaycast(GetWorld(), EnemyLocation, IdealShootingPosition, HitLocation)) {
		ShootingPositionVector = HitLocation - SoldierLocation;
	}
	else {
		ShootingPositionVector = IdealShootingPosition - SoldierLocation;
	}
	
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), SoldierLocation, ShootingPositionVector, NULL);

	FVector ShootingPositionLocalDir;
	if (path) {
		if (path->PathPoints.Num() > 1)
			ShootingPositionLocalDir = path->PathPoints[1];
		else
			ShootingPositionLocalDir = Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsVector("ShootingPosition");
	}

	ShootingPositionVector = ShootingPositionLocalDir - SoldierLocation;

	//DrawDebugPoint(GetWorld(), SoldierLocation + ShootingPositionVector, 32, FColor::Cyan);
}

void UFlockingComponent::UpdateMovementVector()
{

	AlignementVector = AlignementVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
	AlignementVector.Z = 0;
	CohesionVector = CohesionVector/*.GetSafeNormal(DefaultNormalizeVectorTolerance)*/;
	CohesionVector.Z = 0;
	SeparationVector = SeparationVector/*.GetSafeNormal(DefaultNormalizeVectorTolerance)*/;
	SeparationVector.Z = 0;
	WallAvoidanceVector = WallAvoidanceVector/*.GetSafeNormal(DefaultNormalizeVectorTolerance)*/;
	WallAvoidanceVector.Z = 0;
	ObjectifVector = ObjectifVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
	ObjectifVector.Z = 0;
	ShootingPositionVector = ShootingPositionVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
	ShootingPositionVector.Z = 0;

	MovementVector = AlignementVector * AlignementWeight
		+ CohesionVector * CohesionWeight
		+ SeparationVector * SeparationWeight
		+ WallAvoidanceVector * WallAvoidanceWeight
		+ ObjectifVector * ObjectifWeight
		+ ShootingPositionVector * ShootingPositionWeight;

	MovementVector = MovementVector.GetSafeNormal(DefaultNormalizeVectorTolerance);
	MovementVector.Z = 0;
}

void UFlockingComponent::UpdateWallAvoidanceVector()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector Offset = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetActorForwardVector().GetSafeNormal() * RadiusForWallAvoidance;//{ RadiusForWallAvoidance, 0.f, 0.f };
	FVector HitLocation{};
	float AnglePerRay = 360.f / NumberOfRayForWallAvoidance;
	int NbOfHit = 0;
	for (int i = 0; i < NumberOfRayForWallAvoidance; i++) {
		Offset = Offset.RotateAngleAxis(AnglePerRay * i, { 0, 0, 1 });
		if (navSys->NavigationRaycast(GetWorld(), SoldierLocation, SoldierLocation + Offset, HitLocation)) {
			//DrawDebugPoint(GetWorld(), HitLocation, 10, FColor::Red);
			FVector Separation = SoldierLocation - HitLocation;
			if(Separation.Size()>0)WallAvoidanceVector += Separation.GetSafeNormal(DefaultNormalizeVectorTolerance) / FMath::Abs(Separation.Size() - BoidPhysicalRadius);
			NbOfHit++;
		}
		DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + Offset, FColor::Purple);
	}

	const FVector WallAvoidanceForceComponent = WallAvoidanceVector * 50;
	if(NbOfHit)
		WallAvoidanceVector += WallAvoidanceForceComponent + WallAvoidanceVector * (5 / NbOfHit);
}

void UFlockingComponent::DrawDebug()
{
	FVector SoldierLocation = Cast<ASoldier>(Cast<AAIGeneralController>(GetOwner())->GetPawn())->GetLocation();
	SoldierLocation.Z += 100;
	FVector TempFlockPos = Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsVector("FlockingLocation");
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + AlignementVector * AlignementWeight * 200, FColor::Green,false,-1.f,'\000',10); /*Alignement vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + CohesionVector * CohesionWeight * 200, FColor::Blue, false, -1.f, '\000', 10); /*Cohesion vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + SeparationVector * SeparationWeight * 200, FColor::Red, false, -1.f, '\000', 10); /*Separation vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + ObjectifVector * ObjectifWeight * 200, FColor::Yellow, false, -1.f, '\000', 10); /*Objectif vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + WallAvoidanceVector * WallAvoidanceWeight * 200, FColor::Purple, false, -1.f, '\000', 10); /*Wall avoidance vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + ShootingPositionVector * ShootingPositionWeight * 200, FColor::Cyan, false, -1.f, '\000', 10); /*Wall avoidance vector*/
	DrawDebugLine(GetWorld(), SoldierLocation, SoldierLocation + MovementVector, FColor::Black); /*Movement vector*/
	DrawDebugPoint(GetWorld(), Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation(), 12, FColor::Purple);
	DrawDebugPoint(GetWorld(), Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsVector("ShootingPosition"), 12, FColor::Cyan);
	DrawDebugPoint(GetWorld(), TempFlockPos, 12, FColor::Black);
}

void UFlockingComponent::UpdateFlockingPosition(float DeltaSeconds)
{
	ResetVectors();

	UpdateNeighbourhood();

	UpdateAlignementVector();

	if (SeenBoids.Num() > 0) { //because this involve /0
		UpdateCohesionVector();

		UpdateSeparationVector();
	}

	UpdateWallAvoidanceVector();

	if (Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsBool("is_attacking")) {
		UpdateShootingPositionVector();
	}
	else {
		UpdateObjectifVector();
	}

	UpdateMovementVector();

	//TODO Check if the new movement vector is not too opposit to the previous one


	float MaxSpeed = Cast<AAIGeneralController>(GetOwner())->GetPawn()->GetMovementComponent()->GetMaxSpeed();
	MovementVector = MovementVector.GetSafeNormal(DefaultNormalizeVectorTolerance) * MovementVectorScale;

	if (IsFlockingPositionValid()) {
		//Flocking Location Set in IsFlockingPositionValid()
	}
	else if(!Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsBool("is_attacking")){
		FVector RealObjectifLocation = Cast<AAIGeneralController>(GetOwner())->GetObjectifLocation();
		Cast<AAIGeneralController>(GetOwner())->get_blackboard()->SetValueAsVector("FlockingLocation", RealObjectifLocation);
	}
	else {
		Cast<AAIGeneralController>(GetOwner())->get_blackboard()->SetValueAsVector("FlockingLocation", Cast<AAIGeneralController>(GetOwner())->get_blackboard()->GetValueAsVector("ShootingPosition"));
	}

	if(DoDrawDebug)DrawDebug();
}

bool UFlockingComponent::IsFlockingPositionValid()
{
	FVector HitLocation{};

	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector startLocation = Cast<AAIGeneralController>(GetOwner())->GetPawn()->GetActorLocation();
	FVector endLocation = startLocation + MovementVector;

	if (navSys->NavigationRaycast(GetWorld(), startLocation, endLocation, HitLocation)) {
		return false;
	}
	//DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Green);
	//DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation() + MovementVector, FColor::Blue);
	//DrawDebugPoint(GetWorld(), HitLocation, 35, FColor::Red);
	Cast<AAIGeneralController>(GetOwner())->get_blackboard()->SetValueAsVector("FlockingLocation", HitLocation);
	return true;
}

// Called every frame
void UFlockingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

