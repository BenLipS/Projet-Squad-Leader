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
enum class MissionPriority : uint8{ 
	eURGENT,
	eMIDDLE,
	eBASIC
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

	virtual void Init(int _id, MissionPriority _p) noexcept;

	void SetState(MissionState _state) noexcept { m_state = _state; }

protected:

	UPROPERTY()
		MissionState m_state;

	UPROPERTY()
		int m_id;

	UPROPERTY()
		MissionPriority m_priority;

private:
	void SetPriority(MissionPriority _p) noexcept { m_priority = _p; }
	void SetId(int _id) noexcept { m_id = _id; }
};