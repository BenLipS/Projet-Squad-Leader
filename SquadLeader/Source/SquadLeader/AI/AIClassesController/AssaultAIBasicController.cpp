// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultAIBasicController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"

void AAssaultAIBasicController::Tick(float DeltaSeconds) {

	if (SeenEnemySoldier.Num() >= NUmberOfEnemyToLaunchAGrenade) {
		FVector MeanEnmyPos = FVector::ZeroVector;
		for (ASoldier* Enemy : SeenEnemySoldier) {
			MeanEnmyPos += Enemy->GetLocation();
		}
		MeanEnmyPos = MeanEnmyPos.GetSafeNormal() * (MeanEnmyPos.Size() / SeenEnemySoldier.Num());
		FVector LaunchDir = MeanEnmyPos - GetPawn()->GetActorLocation();

		LaunchProjectileYawAdjust = LaunchDir.Rotation().Yaw - GetPawn()->GetActorForwardVector().Rotation().Yaw;
		
		//DrawDebugPoint(GetWorld(), MeanEnmyPos, 32, FColor::Green, false, 10.f);
		/*DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 1000, FColor::Red);
		DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation() + LaunchDir, FColor::Yellow);*/

		Blackboard->SetValueAsBool("LaunchGrenade", true);
	}
	
	Super::Tick(DeltaSeconds);
}