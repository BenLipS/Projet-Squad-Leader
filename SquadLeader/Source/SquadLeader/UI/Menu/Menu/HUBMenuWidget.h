#pragma once

#include "CoreMinimal.h"
#include "../MenuWidget.h"
#include "../MenuLayout/ClassSelectionMenuLayout.h"
#include "HUBMenuWidget.generated.h"

class AGameParam;

UCLASS()
class SQUADLEADER_API UHUBMenuWidget : public UMenuWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UClassSelectionMenuLayout* ClassSelectionLayout;

	UFUNCTION(BlueprintCallable)
	void InitFromGameparam(TSubclassOf<AGameParam> gameParam);

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

protected:
	virtual void SynchronizeProperties() override;
};
