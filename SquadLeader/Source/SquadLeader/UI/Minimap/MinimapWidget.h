#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "../SL_UserWidget.h"
#include "../Interface/MinimapInterface.h"
#include "../Interface/MinimapDelegateInterface.h"
#include "MinimapWidget.generated.h"

class ASoldier;
class AControlArea;
class UPointOfInterestWidget;

// !! WARNING !!
// We suppose the minimap is a square ONLY

UENUM()
enum class MapShapePolicy : uint8 {
	eCIRCLE,
	eSQUARE
};

UCLASS()
class SQUADLEADER_API UMinimapWidget : public USL_UserWidget, public IMinimapInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Dimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Zoom;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* MinimapSoldierOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* MinimapControlAreaOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* PingOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* MinimapImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* PlayerIconImage;

	// Max length a POI can be from the player icon to be visible
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IconMaxLengthVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	MapShapePolicy MapShape;

//////////////// Points of interest
	TArray<UPointOfInterestWidget*> POIList;

// Squad
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> SquadIconWidgetClass;

// Player Allies
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> PlayerAllieIconWidgetClass;

// AI Allies
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> AIAllieIconWidgetClass;

// Ennemies
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> EnnemyIconWidgetClass;

// Control Area
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> ControlAreaIconWidgetClass;

// Ping
	UPROPERTY(EditAnywhere, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> PingIconWidgetClass;

public:
	UMinimapWidget(const FObjectInitializer& _ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetMapShape(MapShapePolicy MapShapeIn);

	UFUNCTION(BlueprintCallable)
	void SetIconMaxLengthVisibility(float LenghtIn);

	//-----USL_UserWidget-----

	void SetupDelegateToObject_Implementation(UObject* _ObjectIn) override;

	//-----IMinimapDelegateInterface-----
	virtual void OnSoldierAddedToTeam(ASoldier* _Soldier) override;
	virtual void OnSoldierRemovedFromTeam(ASoldier* _Soldier) override;
	virtual void OnControlAreaAdded(AControlArea* _ControlArea) override;
	virtual void OnPingAdded(FVector2D PosPingMinimap) override;
	virtual void OnUpdatePOIs() override;

	virtual void OnFullMapDisplayBegin();
	virtual void OnFullMapDisplayEnd();
};