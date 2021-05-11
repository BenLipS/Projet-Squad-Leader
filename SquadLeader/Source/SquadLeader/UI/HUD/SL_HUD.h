#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "SL_HUD.generated.h"

UCLASS()
class SQUADLEADER_API ASL_HUD : public AHUD

{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TArray<TSubclassOf<class USL_UserWidget>> WidgetClassList;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TArray<USL_UserWidget*> WidgetList;

public:
	ASL_HUD() = default;

	//Must be called after it is created
	void InitHUD();

protected:
	virtual void OnInitHUD();

protected:
	virtual void BeginPlay() override;
};