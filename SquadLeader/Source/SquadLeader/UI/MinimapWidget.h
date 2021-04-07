#pragma once

#include "CoreMinimal.h"
#include "SL_UserWidget.h"
#include "Engine/Canvas.h"
#include "Components/Image.h"
#include "Interface/MinimapInterface.h"
#include "Interface/MinimapDelegateInterface.h"
#include "MinimapWidget.generated.h"

UCLASS()
class SQUADLEADER_API UMinimapWidget : public USL_UserWidget, public IMinimapInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* MinimapImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* PlayerIconImage;

public:
	UMinimapWidget(const FObjectInitializer& _ObjectInitializer);

	void SetupDelegateToObject(UObject* _ObjectIn) override;

	//-----IMinimapDelegateInterface-----
	virtual void OnTeamPositionsChanged(const TArray<FVector2D>& _Positions) override;
};