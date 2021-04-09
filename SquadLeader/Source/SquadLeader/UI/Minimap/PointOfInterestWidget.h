#pragma once

#include "CoreMinimal.h"
#include "../SL_UserWidget.h"
#include "SquadLeader/Soldiers/Soldier.h"
#include "PointOfInterestWidget.generated.h"

UCLASS()
class SQUADLEADER_API UPointOfInterestWidget : public USL_UserWidget
{
	GENERATED_BODY()

public:
	ASoldier* OwningSoldier;
};
