#include "MinimapWidget.h"
#include "PointOfInterestWidget.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayer.h"
#include "Components/OverlaySlot.h"
#include "SquadLeader/AI/AISquadManager.h"

UMinimapWidget::UMinimapWidget(const FObjectInitializer& _ObjectInitializer) : USL_UserWidget(_ObjectInitializer),
Dimensions{ 10'000.f },
Zoom{ 1.f },
POIList{}
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

	if (POI)
	{
		// Put the POI to the center of the minimap as the base position
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(MinimapOverlay->AddChild(POI)); OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
		
		POI->OwningSoldier = _Soldier;
		POIList.Add(POI);
	}
}

void UMinimapWidget::OnSoldierRemovedFromTeam(ASoldier* _Soldier)
{
	// Remove any widget whose actor ref match _Soldier
	POIList.RemoveAll([&_Soldier](UPointOfInterestWidget* POI) { return POI->OwningSoldier == _Soldier; });

	// TODO do I have to remove from viewport ?
}

void UMinimapWidget::OnUpdateTeamPositions()
{
	APawn* Player = GetOwningPlayerPawn();
	
	if (!Player)
		return;

	const FVector2D PlayerPosition = FVector2D{ Player->GetActorLocation().X, Player->GetActorLocation().Y };

	for (UPointOfInterestWidget* POI : POIList)
	{
		const FVector2D SoldierPosition = FVector2D{ POI->OwningSoldier->GetActorLocation().X, POI->OwningSoldier->GetActorLocation().Y };

		// Diff position between soldier and player
		const float Coeff = Dimensions * Zoom / MinimapImage->GetDesiredSize().X; // Assume MinimapImage is 
		const float DiffX = (PlayerPosition.X - SoldierPosition.X) / Coeff;
		const float DiffY = (SoldierPosition.Y - PlayerPosition.Y) / Coeff; // Implicit * -1
		const FVector2D DiffVec = { DiffX, DiffY };

		// Angle between soldier and player (center of the minimap)
		const float Angle = FMath::Atan2(/* 0.f*/ - DiffY, /* 0.f*/ - DiffX);
		const float Length = FMath::Clamp(DiffVec.Size(), 0.f, IconMaxLengthVisibility);

		const FVector2D SoldierPosOnMinimap = -FVector2D{ FMath::Sin(Angle) * Length, FMath::Cos(Angle) * Length };
		POI->SetRenderTranslation(SoldierPosOnMinimap);

	}
}