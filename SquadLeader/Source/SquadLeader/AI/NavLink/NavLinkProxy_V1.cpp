// Fill out your copyright notice in the Description page of Project Settings.


#include "NavLinkProxy_V1.h"


ANavLinkProxy_V1::ANavLinkProxy_V1() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ANavLinkProxy_V1::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ANavLinkProxy_V1::Jump(AActor* Agent, const FVector& Destination) {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(20, 1.f, FColor::Blue, TEXT("Start to jump."));
}