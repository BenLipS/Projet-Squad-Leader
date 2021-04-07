#include "MinimapWidget.h"

UMinimapWidget::UMinimapWidget(const FObjectInitializer& _ObjectInitializer) : USL_UserWidget(_ObjectInitializer)
{
}

void UMinimapWidget::SetupDelegateToObject(UObject* _ObjectIn)
{
	if (IMinimapDelegateInterface* MinimapDelegateInterface = Cast<IMinimapDelegateInterface>(_ObjectIn); MinimapDelegateInterface)
	{
		MinimapDelegateInterface->AddMinimapDelegate(this);
	}
}

void UMinimapWidget::OnTeamPositionsChanged(const TArray<FVector2D>& _Positions)
{
	int pfk = 0;
	++pfk;
	++pfk;

	// Faire l affichage ici
}