#pragma once

#include "CoreMinimal.h"
#include "../MenuWidget.h"
#include "../MenuLayout/ClassSelectionMenuLayout.h"
#include "../MenuItem/MenuList/MenuListInfoStat.h"
#include "../../Interface/HUBInterface.h"
#include "HUBMenuWidget.generated.h"

class AGameParam;

UCLASS()
class SQUADLEADER_API UHUBMenuWidget : public UMenuWidget,
	public IHUBInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UClassSelectionMenuLayout* ClassSelectionLayout;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuListInfoStat* Team1Players;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuListInfoStat* Team2Players;

	UFUNCTION(BlueprintCallable)
	void InitFromGameparam(TSubclassOf<AGameParam> gameParam);

public:
	virtual void SetupDelegateToObject_Implementation(UObject* ObjectIn);

protected:
	virtual void SynchronizeProperties() override;

	//-----IHUBInterface-----
public:
	virtual void ClearPlayerList1() override;
	virtual void ClearPlayerList2() override;
	virtual void AddPlayerList1(TMap<FString, FString> statsIn) override;
	virtual void AddPlayerList2(TMap<FString, FString> statsIn) override;
};
