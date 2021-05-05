#pragma once

#include "Core.h"
#include "SoldierPrimarySpawn.h"
#include "../Soldiers/SoldierTeam.h"
#include "Net/UnrealNetwork.h"
#include "SoldierSecondarySpawn.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ASoldierSecondarySpawn : public ASoldierPrimarySpawn
{
	GENERATED_BODY()

public:
	ASoldierSecondarySpawn();

public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		void SetUsed(bool _canBeUsed) { canBeUsed = _canBeUsed; }
};
