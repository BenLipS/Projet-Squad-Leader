#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "SoldierPlayer.generated.h"

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

protected:
	void SetAbilitySystemComponent();
	void BindASCInput();

	bool ASCInputBound;
};
