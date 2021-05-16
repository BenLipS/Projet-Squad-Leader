#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "PointOfInterestWidget.generated.h"

UCLASS()
class SQUADLEADER_API UPointOfInterestWidget : public USL_UserWidget
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<AActor> OwningActor;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Visibility")
	bool bPersistant = false;
};
