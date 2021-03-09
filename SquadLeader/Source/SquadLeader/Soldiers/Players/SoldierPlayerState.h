#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Soldier.h"
#include "AbilitySystemInterface.h"
#include "../../AbilitySystem/Soldiers/AttributeSetSoldier.h"
#include "../../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../Interface/Teamable.h"
#include "SoldierPlayerState.generated.h"

UCLASS()
class SQUADLEADER_API ASoldierPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamable
{
	GENERATED_BODY()

public:
	ASoldierPlayerState();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	void BeginPlay() override;

//////////////// Teamable
protected:
	UPROPERTY(Replicated)
		TSubclassOf<ASoldierTeam> Team = nullptr;  // only server can replicate it
public:
	virtual TSubclassOf<ASoldierTeam> GetTeam() override;
	virtual bool SetTeam(TSubclassOf<ASoldierTeam> _Team) override;

//////////////// Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemSoldier* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute Set", meta = (AllowPrivateAccess = "true"))
	UAttributeSetSoldier* AttributeSet;

public:
	UAbilitySystemSoldier* GetAbilitySystemComponent() const override;
	UAttributeSetSoldier* GetAttributeSet() const;
};
