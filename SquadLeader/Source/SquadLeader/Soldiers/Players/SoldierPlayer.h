#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierPlayer.generated.h"
class AAISquadManager;

UCLASS()
class SQUADLEADER_API ASoldierPlayer : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierPlayer(const FObjectInitializer& _ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	void PossessedBy(AController* _newController) override;
	void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly, Category = "SquadManager")
	TSubclassOf<AAISquadManager> AISquadManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = "SquadManager")
	AAISquadManager* SquadManager;

public:
	AAISquadManager* GetSquadManager();

protected:
	void SetAbilitySystemComponent();
	void BindASCInput();

	bool ASCInputBound;

public:  // Respawn
	virtual FVector GetRespawnPoint() override;
};
