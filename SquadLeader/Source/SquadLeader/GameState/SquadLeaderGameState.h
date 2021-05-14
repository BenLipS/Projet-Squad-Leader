#pragma once

class AControlAreaManager;
class ASoldierTeam;

#include "Core.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "SquadLeaderGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoldierTeamAddedToList, ASoldierTeam*, NewSoldierTeam);

UCLASS()
class SQUADLEADER_API ASquadLeaderGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASquadLeaderGameState();

	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// storage of global data for all the game
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "GameModeData")
	AControlAreaManager* ControlAreaManager;
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "GameModeData")
	TArray<ASoldierTeam*> SoldierTeamCollection;

public:
	UFUNCTION()
	void AddSoldierTeam(ASoldierTeam* _SoldierTeam);
	UFUNCTION()
		TArray<ASoldierTeam*> GetSoldierTeamCollection();
	UFUNCTION()
		ASoldierTeam* GetSoldierTeamByID(int TeamId);
	
	UFUNCTION()
	AControlAreaManager* GetControlAreaManager();
	UFUNCTION()
	void SetControlAreaManager(AControlAreaManager* _ControlAreaManager);

	UPROPERTY()
	FSoldierTeamAddedToList OnSoldierTeamAddedToList;
};
