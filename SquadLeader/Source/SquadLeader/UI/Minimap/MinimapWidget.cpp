#include "MinimapWidget.h"
#include "PointOfInterestWidget.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/ControlArea/ControlArea.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayer.h"
#include "Components/OverlaySlot.h"
#include "SquadLeader/AI/AISquadManager.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Blueprint/WidgetTree.h"

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsPlayerCentered && IsValid(MaterialCollection))
	{
		FVector ActorPosition = GetOwningPlayerPawn()->GetActorLocation();

		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialCollection, FName("X"), ActorPosition.X);

		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialCollection, FName("Y"), ActorPosition.Y);
	}

	auto ActorRotation = GetOwningPlayer()->GetPawn<ASoldier>()->GetMesh()->GetComponentRotation();
	PlayerIconImage->SetRenderTransformAngle(ActorRotation.Yaw);
}

bool UMinimapWidget::IsInDisplay(FVector2D DifferenceVec, FVector2D SizeIn)
{
	FVector2D size;
	if (IsValid(ScaleBoxMap))
		size = ScaleBoxMap->GetTickSpaceGeometry().GetLocalSize();
	else
		size = GetDesiredSize();
	switch (MapShape)
	{
	case MapShapePolicy::eCIRCLE :
		return (DifferenceVec.Size() + (SizeIn.Y / 2.f)) <= (size.Y / 2.f);
		break;

	case MapShapePolicy::eSQUARE :
		return DifferenceVec.GetAbs() + SizeIn.GetAbs() <= size.GetAbs();
		break;

	default:
		return false;
	}
}

UMinimapWidget::UMinimapWidget(const FObjectInitializer& _ObjectInitializer) : USL_UserWidget(_ObjectInitializer),
Dimensions{ 10'000.f },
POIList{}
{
}

void UMinimapWidget::SetMapShape(MapShapePolicy MapShapeIn)
{
	MapShape = MapShapeIn;
}

/*void UMinimapWidget::SetIconMaxLengthVisibility(float LenghtIn)
{
	IconMaxLengthVisibility = LenghtIn;
}*/

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
	//
	if (IsValid(PingIconWidgetClass))
	{
		if (!PingPOI)
		{
			PingPOI = CreateWidget<UPointOfInterestWidget>(PingOverlay, PingIconWidgetClass);
			if (PingPOI)
			{
				// Put the POI to the center of the minimap as the base position
				if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PingOverlay->AddChild(PingPOI)); OverlaySlot)
				{
					OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
					OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
				}
			}
		}
	}
	PingLocation = PosPingMinimap;
	bPingActive = true;
}

void UMinimapWidget::OnPingDestroyed()
{
	bPingActive = false;
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
	const float Coeff = Dimensions * UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, FName("Zoom")) / ScaleBoxMap->GetTickSpaceGeometry().GetLocalSize().Y; // Assume MinimapImage is 
	
	FVector2D CenterScreen;
	if (bIsPlayerCentered)
	{
		CenterScreen = PlayerPosition;
	}
	else
	{
		CenterScreen = FVector2D{10000.f, 10000.f };
		const float DiffX = (PlayerPosition.X - CenterScreen.X) / Coeff;
		const float DiffY = (PlayerPosition.Y - CenterScreen.Y) / Coeff;
		const FVector2D DiffVec = { DiffY, -DiffX };
		PlayerIconImage->SetRenderTranslation(DiffVec);
	}

	//-----Ping-----

	if (IsValid(PingPOI))
	{

		const float DiffX = (CenterScreen.X - PingLocation.X) / Coeff;
		const float DiffY = (PingLocation.Y - CenterScreen.Y) / Coeff; // Implicit * -1
		const FVector2D DiffVec = { DiffX, DiffY };

		// The POI is too far from the player
		if (!bPingActive || (!PingPOI->bPersistant && !IsInDisplay(DiffVec, PingPOI->GetTickSpaceGeometry().GetLocalSize())))
		{
			PingPOI->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{

			// Angle between soldier and player (center of the minimap)
			const float Angle = FMath::Atan2(/* 0.f*/ -DiffY, /* 0.f*/ -DiffX);
			MapPanel->GetDesiredSize();
			const float Length = FMath::Clamp(DiffVec.Size(), 0.f, (ScaleBoxMap->GetTickSpaceGeometry().GetLocalSize().Y - PingPOI->GetTickSpaceGeometry().GetLocalSize().Y) / 2.f);

			const FVector2D SoldierPosOnMinimap = -FVector2D{ FMath::Sin(Angle) * Length, FMath::Cos(Angle) * Length };
			PingPOI->SetRenderTranslation(SoldierPosOnMinimap);

			PingPOI->SetVisibility(ESlateVisibility::Visible);
		}
	}

	for (UPointOfInterestWidget* POI : POIList)
	{
		const FVector2D ActorPosition = FVector2D{ POI->OwningActor->GetActorLocation().X, POI->OwningActor->GetActorLocation().Y };
		// Diff position between soldier and player
		
		const float DiffX = (CenterScreen.X - ActorPosition.X) / Coeff;
		const float DiffY = (ActorPosition.Y - CenterScreen.Y) / Coeff; // Implicit * -1
		const FVector2D DiffVec = { DiffX, DiffY };

		// The POI is too far from the player
		if (!POI->bPersistant && !IsInDisplay(DiffVec, POI->GetTickSpaceGeometry().GetLocalSize()))
		{
			POI->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		// Angle between soldier and player (center of the minimap)
		const float Angle = FMath::Atan2(/* 0.f*/ - DiffY, /* 0.f*/ - DiffX);
		MapPanel->GetDesiredSize();
		const float Length = FMath::Clamp(DiffVec.Size(), 0.f, (ScaleBoxMap->GetTickSpaceGeometry().GetLocalSize().Y - POI->GetTickSpaceGeometry().GetLocalSize().Y) /2.f);

		const FVector2D SoldierPosOnMinimap = -FVector2D{ FMath::Sin(Angle) * Length, FMath::Cos(Angle) * Length };
		POI->SetRenderTranslation(SoldierPosOnMinimap);
		POI->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMinimapWidget::OnFullMapDisplayBegin()
{
	if (IsValid(MinimapToMap))
	{
		PlayAnimationForward(MinimapToMap);
		bIsPlayerCentered = false;
	}
}

void UMinimapWidget::OnFullMapDisplayEnd()
{
	if (IsValid(MinimapToMap))
	{
		PlayAnimationReverse(MinimapToMap);
		bIsPlayerCentered = true;
		PlayerIconImage->SetRenderTranslation(FVector2D::ZeroVector);
	}
}
