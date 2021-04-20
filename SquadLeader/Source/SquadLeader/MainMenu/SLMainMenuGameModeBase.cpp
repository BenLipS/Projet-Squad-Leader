// Fill out your copyright notice in the Description page of Project Settings.

#include "SLMainMenuGameModeBase.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void ASLMainMenuGameModeBase::StartPlay() {
	for (auto PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; PCIterator++)
	{
		if (auto PC = PCIterator->Get(); PC)
		{
			PC->SetShowMouseCursor(true);
			PC->ClientIgnoreMoveInput(true);
			PC->ClientIgnoreLookInput(true);

			FVector2D Center = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2.0f;
			PC->SetMouseLocation(Center.X, Center.Y);
		}
	}

	Super::StartPlay();
}