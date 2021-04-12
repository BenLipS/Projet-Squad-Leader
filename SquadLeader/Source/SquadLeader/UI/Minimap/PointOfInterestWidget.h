#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "PointOfInterestWidget.generated.h"

UCLASS()
class SQUADLEADER_API UPointOfInterestWidget : public USL_UserWidget
{
	GENERATED_BODY()

public:
	AActor* OwningActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Visibility")
	bool bPersistant = false;
};
