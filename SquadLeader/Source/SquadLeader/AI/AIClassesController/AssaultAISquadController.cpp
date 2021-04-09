// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultAISquadController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "BehaviorTree/BehaviorTree.h"

AAssaultAISquadController::AAssaultAISquadController() {
	setup_BehaviorTree();
}

void AAssaultAISquadController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//TODO Make a function with that LaunchGrenade()
	if (SeenEnemySoldier.Num() >= NUmberOfEnemyToLaunchAGrenade && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.LaunchProjectile.Grenade")))) {
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

		StopCurrentBehavior = true;
		Blackboard->SetValueAsBool("LaunchGrenade", true);
	}

	//TODO Make a function with that OverHeat()
	if (SeenEnemySoldier.Num() >= NUmberOfEnemyToOverHeat && !Cast<ASoldier>(GetPawn())->IsInCooldown(FGameplayTag::RequestGameplayTag(FName("Cooldown.Ability.OverheatingWeapon")))) {
		StopCurrentBehavior = true;
		Blackboard->SetValueAsBool("OverHeat", true);
	}
}

void AAssaultAISquadController::setup_BehaviorTree()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/BT_Squad_Classes/BT_AISquad_Assault.BT_AISquad_Assault'"));
	if (obj.Succeeded())
		m_behaviorTree = obj.Object;
}

