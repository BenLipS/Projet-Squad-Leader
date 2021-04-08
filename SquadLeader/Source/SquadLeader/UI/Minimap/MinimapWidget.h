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
class UPointOfInterestWidget;

USTRUCT()
struct FPointOfInterestSoldier
{
	GENERATED_BODY()

	ASoldier* Soldier = nullptr;
	UPointOfInterestWidget* Widget = nullptr;

	FPointOfInterestSoldier() = default;
	FPointOfInterestSoldier(ASoldier* _Soldier, UPointOfInterestWidget* _Widget)
	{
		Soldier = _Soldier;
		Widget = _Widget;
	}
};

// !! WARNING !!
// We suppose the minimap is a square ONLY

UCLASS()
class SQUADLEADER_API UMinimapWidget : public USL_UserWidget, public IMinimapInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Dimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Zoom;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* MinimapOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* MinimapImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* PlayerIconImage;

//////////////// Points of interest
	TArray<FPointOfInterestSoldier> SoldierList;

// Squad
	UPROPERTY(EditDefaultsOnly, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> SquadIconWidgetClass;

// Allies
	UPROPERTY(EditDefaultsOnly, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> AllieIconWidgetClass;

// Ennemies
	UPROPERTY(EditDefaultsOnly, Category = "PointOfInterest")
	TSubclassOf<UPointOfInterestWidget> EnnemyIconWidgetClass;

public:
	UMinimapWidget(const FObjectInitializer& _ObjectInitializer);

	void SetupDelegateToObject(UObject* _ObjectIn) override;

	//-----IMinimapDelegateInterface-----
	virtual void OnSoldierAddedToTeam(ASoldier* _Soldier) override;
	virtual void OnSoldierRemovedFromTeam() override;

	virtual void OnUpdateTeamPositions() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewTeamPosition();
};