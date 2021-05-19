#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ScaleBox.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "../SL_UserWidget.h"
#include "../Interface/MinimapInterface.h"
#include "../Interface/MinimapDelegateInterface.h"
#include "Animation/WidgetAnimation.h"
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
	static const int MAPSIZE = 33570;
	static const int MAPCENTER_X = 25946;
	static const int MAPCENTER_Y = 28950;
	static const int MAPMIN_X = 9353;
	static const int MAPMIN_Y = 12363;


protected:
	UPROPERTY()
	UPointOfInterestWidget* PingPOI = nullptr;

	UPROPERTY()
	FVector2D PingLocation = FVector2D::ZeroVector;

	UPROPERTY()
	bool bPingActive = false;

	UPROPERTY()
	bool bMapInteractive = false;

	UPROPERTY()
	bool bMapKeyPressed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Dimensions;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Zoom;*/

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
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MapPanel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MapPanelContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScaleBox* ScaleBoxMap;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* MinimapToMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UMaterialParameterCollection* MaterialCollection;

	// Max length a POI can be from the player icon to be visible
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IconMaxLengthVisibility;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	MapShapePolicy MapShape = MapShapePolicy::eCIRCLE;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPlayerCentered = true;

//////////////// Points of interest
	UPROPERTY()
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

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, 	const FWidgetStyle& InWidgetStyle, 	bool bParentEnabled) const override;

	bool IsInDisplay(FVector2D DifferenceVec, FVector2D Size);

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void SetInteractivity(bool InInteractivity);

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UMinimapWidget(const FObjectInitializer& _ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetMapShape(MapShapePolicy MapShapeIn);

	/*UFUNCTION(BlueprintCallable)
	void SetIconMaxLengthVisibility(float LengthIn);*/

	//-----USL_UserWidget-----

	void SetupDelegateToObject_Implementation(UObject* _ObjectIn) override;

	//-----IMinimapDelegateInterface-----
	virtual void OnSoldierAddedToTeam(ASoldier* _Soldier) override;
	virtual void OnSoldierRemovedFromTeam(ASoldier* _Soldier) override;
	virtual void OnControlAreaAdded(AControlArea* _ControlArea) override;
	virtual void OnPingAdded(FVector2D PosPingMinimap) override;
	virtual void OnPingDestroyed() override;
	virtual void OnUpdatePOIs() override;

	virtual void OnFullMapDisplayBegin() override;
	virtual void OnFullMapDisplayEnd() override;
};