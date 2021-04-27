#include "MinimapWidget.h"
#include "PointOfInterestWidget.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/ControlArea/ControlArea.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayer.h"
#include "Components/OverlaySlot.h"
#include "SquadLeader/AI/AISquadManager.h"

#include "Blueprint/WidgetTree.h"

UMinimapWidget::UMinimapWidget(const FObjectInitializer& _ObjectInitializer) : USL_UserWidget(_ObjectInitializer),
Dimensions{ 10'000.f },
Zoom{ 1.f },
IconMaxLengthVisibility{ 130.f },
POIList{}
{
}

void UMinimapWidget::SetMapShape(MapShapePolicy MapShapeIn)
{
	MapShape = MapShapeIn;
}

void UMinimapWidget::SetIconMaxLengthVisibility(float LenghtIn)
{
	IconMaxLengthVisibility = LenghtIn;
}

void UMinimapWidget::SetupDelegateToObject_Implementation(UObject* _ObjectIn)
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
		POI = CreateWidget<UPointOfInterestWidget>(MinimapSoldierOverlay, EnnemyIconWidgetClass);
	}
	else if (!!Cast<ASoldierPlayer>(_Soldier))// Soldier is a allie player
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapSoldierOverlay, PlayerAllieIconWidgetClass);
	}
	else if (Player->GetSquadManager()->HasSoldier(_Soldier)) // Soldier is part of the player's squad - // TODO: Review the test
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapSoldierOverlay, SquadIconWidgetClass);
	}
	else // Soldier is an AI allie
	{
		POI = CreateWidget<UPointOfInterestWidget>(MinimapSoldierOverlay, AIAllieIconWidgetClass);
	}

	if (POI)
	{
		// Put the POI to the center of the minimap as the base position
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(MinimapSoldierOverlay->AddChild(POI)); OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
		
		POI->OwningActor = _Soldier;
		POIList.Add(POI);
	}
}

void UMinimapWidget::OnControlAreaAdded(AControlArea* _ControlArea)
{
	UPointOfInterestWidget* POI = CreateWidget<UPointOfInterestWidget>(MinimapControlAreaOverlay, ControlAreaIconWidgetClass);

	if (POI)
	{
		// Put the POI to the center of the minimap as the base position
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(MinimapControlAreaOverlay->AddChild(POI)); OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}

		POI->OwningActor = _ControlArea;

		POIList.Add(POI);
	}
}

void UMinimapWidget::OnPingAdded(FVector2D PosPingMinimap)
{
	//creer un widget
	UPointOfInterestWidget* POI;

	//
	if (IsValid(PingIconWidgetClass))
	{
		POI = CreateWidget<UPointOfInterestWidget>(PingOverlay, PingIconWidgetClass);

		if (POI)
		{
			// Put the POI to the center of the minimap as the base position
			if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PingOverlay->AddChild(POI)); OverlaySlot)
			{
				OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			}
		}
	}
	

}

void UMinimapWidget::OnSoldierRemovedFromTeam(ASoldier* _Soldier)
{
	// Remove any widget whose actor ref match _Soldier
	POIList.RemoveAll([&_Soldier](UPointOfInterestWidget* POI) { return POI->OwningActor == _Soldier; });

	// TODO do I have to remove from viewport ?
}

void UMinimapWidget::OnUpdatePOIs()
{
	APawn* Player = GetOwningPlayerPawn();
	
	if (!Player)
		return;

	const FVector2D PlayerPosition = FVector2D{ Player->GetActorLocation().X, Player->GetActorLocation().Y };

	for (UPointOfInterestWidget* POI : POIList)
	{
		const FVector2D ActorPosition = FVector2D{ POI->OwningActor->GetActorLocation().X, POI->OwningActor->GetActorLocation().Y };

		// Diff position between soldier and player
		const float Coeff = Dimensions * Zoom / MinimapImage->GetDesiredSize().X; // Assume MinimapImage is 
		const float DiffX = (PlayerPosition.X - ActorPosition.X) / Coeff;
		const float DiffY = (ActorPosition.Y - PlayerPosition.Y) / Coeff; // Implicit * -1
		const FVector2D DiffVec = { DiffX, DiffY };

		// The POI is too far from the player
		if (!POI->bPersistant && DiffVec.Size() > IconMaxLengthVisibility)
		{
			POI->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		// Angle between soldier and player (center of the minimap)
		const float Angle = FMath::Atan2(/* 0.f*/ - DiffY, /* 0.f*/ - DiffX);
		const float Length = FMath::Clamp(DiffVec.Size(), 0.f, IconMaxLengthVisibility);

		const FVector2D SoldierPosOnMinimap = -FVector2D{ FMath::Sin(Angle) * Length, FMath::Cos(Angle) * Length };
		POI->SetRenderTranslation(SoldierPosOnMinimap);
		POI->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMinimapWidget::OnFullMapDisplayBegin()
{
}

void UMinimapWidget::OnFullMapDisplayEnd()
{
}
