#pragma once

#include "Core.h"
#include "GameFramework/PlayerState.h"
#include "../Soldier.h"
#include "KillStats.h"
#include "AbilitySystemInterface.h"
#include "../../AbilitySystem/Soldiers/AttributeSetSoldier.h"
#include "../../AbilitySystem/Soldiers/AbilitySystemSoldier.h"
#include "../Interface/Teamable.h"
#include "../../MainMenu/PlayerParam/PlayerParam.h"
#include "SoldierPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatAttributeChanged, float, newValue);

UCLASS()
class SQUADLEADER_API ASoldierPlayerState : public APlayerState, public IAbilitySystemInterface, public ITeamable
{
	GENERATED_BODY()

public:
	FFloatAttributeChanged OnHealthChanged;
	FFloatAttributeChanged OnMaxHealthChanged;
	FFloatAttributeChanged OnShieldChanged;
	FFloatAttributeChanged OnMaxShieldChanged;
	FFloatAttributeChanged OnEXPChanged;
	FFloatAttributeChanged OnEXPLevelUpChanged;

public:
	ASoldierPlayerState();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	void BeginPlay() override;
	void InitializeAttributeChangeCallbacks();

//////////////// Teamable
public:
	virtual ASoldierTeam* GetTeam() override;
	virtual bool SetTeam(ASoldierTeam* _Team) override;

//////////////// Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System Component", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemSoldier* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute Set", meta = (AllowPrivateAccess = "true"))
	UAttributeSetSoldier* AttributeSet;

public:
	UAbilitySystemSoldier* GetAbilitySystemComponent() const override;
	UAttributeSetSoldier* GetAttributeSet() const;

//////////////// Attributes
public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetEXP() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetEXPLevelUp() const;

public:
	int KillingStreak = 0;
	bool IsinKillingStreak = false;

protected:
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ShieldChangedDelegateHandle;
	FDelegateHandle MaxShieldChangedDelegateHandle;
	FDelegateHandle EXPChangedDelegateHandle;
	FDelegateHandle EXPLevelUpChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ShieldChanged(const FOnAttributeChangeData& Data);
	virtual void MaxShieldChanged(const FOnAttributeChangeData& Data);
	virtual void EXPChanged(const FOnAttributeChangeData& Data);
	virtual void EXPLevelUpChanged(const FOnAttributeChangeData& Data);
public:
	void BroadCastAllDatas();

public:
	AKillStats* PersonalRecord;

	UPROPERTY(Replicated)
	TSubclassOf<APlayerParam> PlayerParam;
};
