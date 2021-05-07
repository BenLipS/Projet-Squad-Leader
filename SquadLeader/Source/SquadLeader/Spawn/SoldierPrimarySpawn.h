#pragma once

#include "Core.h"
#include "SoldierSpawn.h"
#include "../Interface/PreInitable.h"
#include "../Soldiers/SoldierTeam.h"
#include "Net/UnrealNetwork.h"
#include "SoldierPrimarySpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierPrimarySpawn : public ASoldierSpawn, public IPreInitable
{
	GENERATED_BODY()

public:
	ASoldierPrimarySpawn();

public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// Pre init launch by the gameMode before the BeginPlay() function
	virtual void PreInitialisation() override;
	virtual int GetPriority() const override;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

protected: // Owner part
	// Server call
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerChangeTeamOwner(ASoldierTeam* _teamOwner);
	UFUNCTION()
		void OnRep_ChangeTeamOwner();
	ASoldierTeam* previousTeamOwner = nullptr;// buffer to know the last owner

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "SpawnCondition")
		ASoldierTeam* teamOwner = nullptr;
	UFUNCTION()
		void UpdateTeamOwner();
};
