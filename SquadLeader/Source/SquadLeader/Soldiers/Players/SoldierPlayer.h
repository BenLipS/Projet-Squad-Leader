#pragma once

#include "CoreMinimal.h"
#include "../Soldier.h"
#include "../AIs/SoldierAI.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PostProcessComponent.h"
#include "SoldierPlayer.generated.h"
class AAISquadManager;

UCLASS()
class SQUADLEADER_API ASoldierPlayer : public ASoldier
{
	GENERATED_BODY()
	
public:
	ASoldierPlayer(const FObjectInitializer& _ObjectInitializer);

public:
	virtual void BeginPlay() override;
	void PossessedBy(AController* _newController) override;
	void OnRep_PlayerState() override;

//////////////// Controllers
protected:
	virtual void LockControls() override;
	virtual void UnLockControls() override;

//////////////// Squad
protected:
	UPROPERTY(EditDefaultsOnly, Category = "SquadManager")
	TSubclassOf<class AAISquadManager> AISquadManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = "SquadManager")
	class AAISquadManager* SquadManager;

	// Number of AIs to add for the next level up
	UPROPERTY(EditDefaultsOnly, Category = "SquadManager")
	FScalableFloat NbAIsForNextLevelUp = 0.f;

public:
	UFUNCTION()
	AAISquadManager* GetSquadManager();

//////////////// Camera
public:
	virtual void LookUp(const float _Val) override;
	virtual void Turn(const float _Val) override;

	virtual void cycleBetweenTeam() override;

//////////////// Post Effects
// Glitch effect
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|PostEffects")
	UPostProcessComponent* PostProcessVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|PostEffects|GlitchEffect")
	UMaterialInterface* MaterialGlitchInterface = nullptr;

	// Material instance for the glitch in this PostProcessVolume
	UPROPERTY(BlueprintReadWrite, Category = "Camera|PostEffects|GlitchEffect")
	UMaterialInstanceDynamic* MaterialGlitchInstance = nullptr;

	// The current weight of the glitch effect
	UPROPERTY(BlueprintReadOnly)
	float WeightGlitchEffect;

	FTimerHandle TimerGlitchReduction;

	// Time between each weight reduction of the glitch effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|PostEffects|GlitchEffect")
	float TimeBetweenReductionGlitch = 0.1f;

	// Multiplier to reduce weight of the glitch effect - Should be lower than 1
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|PostEffects|GlitchEffect", meta = (ClampMin = "0.001", UIMin = "0.999"))
	float ReductionMultiplierGlitch = 0.9f;

	// Minimum weight for smooth glitch reduction before ending the glitch 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|PostEffects|GlitchEffect")
	float MinimumWeightForGlitchReduction = 0.1f;

public:
	virtual void OnBlurredVisionFromJammer(const bool _IsBlurred) override;

	UFUNCTION(BlueprintCallable)
	void SetWeightGlitchEffect(const float _Weight);

	UFUNCTION()
	void StartReducingGlitch();

	UFUNCTION()
	void ReduceGlitch();

	UFUNCTION()
	void EndGlitch();

//////////////// Ability System Component
protected:
	void SetAbilitySystemComponent();
	void BindASCInput();

	bool ASCInputBound = false;

	virtual void LevelUp() override;

public:  // Respawn
	virtual FVector GetRespawnPoint() override;

//////////////// SquadManager data callbacks

	UFUNCTION()
	void OnSquadChanged(const TArray<FSoldierAIData>& newValue);
};
