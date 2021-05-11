#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Mission.generated.h"

/**
 * Mission that will be send to an AI
 */

UENUM()
enum class MissionState : uint8{
	eNOT_STARTED_YET,
	eRUNNING,
	ePAUSED,
	eCOMPLETED,
	eFAILED
};

UENUM()
enum class MissionType : uint8 {
	eATTACK,
	eDEFEND,
	ePATROL,
	eCAPTURE,
	None,
	FormationCircle,
	FormationArrow
};

UENUM()
enum class MissionPriority : uint8{ 
	eNULL,
	eBASIC,
	eMIDDLE,
	eURGENT
};

/**
 * This enum contains the different state of the AI
 */
UENUM()
enum class AIBasicState : uint8 {
	Attacking UMETA(DisplayName = "Attacking"),
	Patroling UMETA(DisplayName = "Patroling"),
	Capturing UMETA(DisplayName = "Capturing"),
	Search UMETA(DisplayName = "Searching"),
	Defend UMETA(DisplayName = "Defending"),
	Moving UMETA(DisplayName = "Moving"),
	Formation UMETA(DisplayName = "Formation"),
};

UCLASS()
class SQUADLEADER_API UMission : public UObject
{
	GENERATED_BODY()

public:
	UMission();
	UMission(int _id, MissionPriority _p);
	virtual ~UMission() = default;

	MissionPriority GetPriority() const noexcept { return m_priority; }

	int GetId() const noexcept { return m_id; }

	MissionState GetMissionState() const noexcept { return m_state; }

	void Init(int _id, MissionPriority _p);

	void SetState(MissionState _state) noexcept { m_state = _state; }

	auto GetMissionType() const noexcept { return m_type; }

protected:

	UPROPERTY()
		MissionState m_state;

	UPROPERTY()
		int m_id;

	UPROPERTY()
		MissionPriority m_priority;

	UPROPERTY()
		MissionType m_type;

private:
	void SetPriority(MissionPriority _p) noexcept { m_priority = _p; }
	void SetId(int _id) noexcept { m_id = _id; }
};