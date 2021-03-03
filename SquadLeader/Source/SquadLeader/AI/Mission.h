#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Mission.generated.h"

/**
 * Mission that will be send to an AI
 */

UENUM()
enum class MissionType : uint8 { None, Defence, Attack, MoveTo };

UCLASS()
class SQUADLEADER_API UMission : public UObject
{
	GENERATED_BODY()

public:
	UMission();

	UPROPERTY()
	MissionType Type;

	FVector Location;
};