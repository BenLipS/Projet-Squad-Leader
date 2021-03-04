#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierPlayer.generated.h"
class UAISquadManager;

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
	TSubclassOf<UAISquadManager> AISquadManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = "SquadManager")
	UAISquadManager* SquadManager;

public:
	UAISquadManager* GetSquadManager();

//////////////// Camera
public:
	virtual void LookUp(const float _Val) override;
	virtual void Turn(const float _Val) override;

//////////////// Ability System Component
protected:
	void SetAbilitySystemComponent();
	void BindASCInput();

	bool ASCInputBound;

public:  // Respawn
	virtual FVector GetRespawnPoint() override;
};
