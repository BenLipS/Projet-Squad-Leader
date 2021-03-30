#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "../AIs/SoldierAI.h"
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

//////////////// Controllers
protected:
	virtual void LockControls() override;
	virtual void UnLockControls() override;

//////////////// Squad
	UPROPERTY(EditDefaultsOnly, Category = "SquadManager")
	TSubclassOf<class AAISquadManager> AISquadManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = "SquadManager")
	class AAISquadManager* SquadManager;

public:
	UFUNCTION()
	AAISquadManager* GetSquadManager();

//////////////// Camera
public:
	virtual void LookUp(const float _Val) override;
	virtual void Turn(const float _Val) override;


	virtual void cycleBetweenTeam() override;


//////////////// Ability System Component
protected:
	void SetAbilitySystemComponent();
	void BindASCInput();

	bool ASCInputBound;

public:  // Respawn
	virtual FVector GetRespawnPoint() override;

//////////////// SquadManager data callbacks

	UFUNCTION()
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue);
};
