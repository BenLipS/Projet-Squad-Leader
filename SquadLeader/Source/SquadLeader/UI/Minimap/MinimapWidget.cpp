#include "MinimapWidget.h"
#include "PointOfInterestWidget.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayer.h"
#include "Components/OverlaySlot.h"
#include "SquadLeader/AI/AISquadManager.h"

UMinimapWidget::UMinimapWidget(const FObjectInitializer& _ObjectInitializer) : USL_UserWidget(_ObjectInitializer), Dimensions{ 10'000.f }, Zoom{ 1.f }, SoldierList{}
{
}

void UMinimapWidget::SetupDelegateToObject(UObject* _ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(_ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
	}
}

void UMinimapWidget::OnSoldierAddedToTeam(ASoldier* _Soldier)
{
	ASoldierPlayer* Player = Cast<ASoldierPlayer>(GetOwningPlayerPawn());
	if (!Player || Cast<ASoldier>(Player) == _Soldier)
		return;

	UPointOfInterestWidget* POI;

	if (_Soldier->GetTeam() != Player->GetTeam()) // Soldier is an enemy
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapOverlay, EnnemyIconWidgetClass);
	}
	else if (Player->GetSquadManager()->HasSoldier(_Soldier)) // Soldier is part of the player's squad
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapOverlay, SquadIconWidgetClass);
	}
	else // Soldier is an allie - not in player's squad
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapOverlay, AllieIconWidgetClass);
	}

	// Create the POI widget, put it to the center of the map then display it
	if (POI)
	{
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(MinimapOverlay->AddChild(POI)); OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
		
		SoldierList.Add(FPointOfInterestSoldier{ _Soldier, POI });
	}
}

void UMinimapWidget::OnSoldierRemovedFromTeam()
{
	// Remove any nullptr from the list
	SoldierList.RemoveAll([](const FPointOfInterestSoldier& POI) { return !POI.Soldier; });
}

void UMinimapWidget::OnUpdateTeamPositions()
{
	APawn* Player = GetOwningPlayerPawn();
	
	if (!Player)
		return;

	const FVector2D PlayerPosition = FVector2D{ Player->GetActorLocation().X, Player->GetActorLocation().Y };

	//for (ASoldier* Soldier : SoldierList)
	for (const FPointOfInterestSoldier& PointOfInterestSoldier : SoldierList)
	{
		const FVector2D SoldierPosition = FVector2D{ PointOfInterestSoldier.Soldier->GetActorLocation().X, PointOfInterestSoldier.Soldier->GetActorLocation().Y };

		// Diff position between soldier and player
		const float Coeff = Dimensions * Zoom / MinimapImage->GetDesiredSize().X; // Assume MinimapImage is 
		const float DiffX = (PlayerPosition.X - SoldierPosition.X) / Coeff;
		const float DiffY = (SoldierPosition.Y - PlayerPosition.Y) / Coeff; // Implicit * -1
		const FVector2D DiffVec = { DiffX, DiffY };

		// Angle between soldier and player (center of the minimap)
		const float Angle = FMath::Atan2(/* 0.f*/ - DiffY, /* 0.f*/ - DiffX);

		const float MaxLength = 130.f; // TODO: expose this property
		const float Length = FMath::Clamp(DiffVec.Size(), 0.f, MaxLength);

		const FVector2D SoldierPosOnMinimap = -FVector2D{FMath::Sin(Angle) * Length, FMath::Cos(Angle) * Length };
		PointOfInterestSoldier.Widget->SetRenderTranslation(SoldierPosOnMinimap);
	}

	OnNewTeamPosition();
}