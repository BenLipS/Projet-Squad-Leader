// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "../Interface/PreInitable.h"
#include "../Soldiers/SoldierTeam.h"
#include "ControlAreaTeamStat.h"
#include "Net/UnrealNetwork.h"
#include "ControlArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FControlAreaIntChanged, int, newInt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FControlAreaFloatChanged, float, newFloat);

UCLASS()
class SQUADLEADER_API AControlArea : public AActor, public IPreInitable
{
	GENERATED_BODY()

public:
	FControlAreaIntChanged OnOwnerChanged;
	FControlAreaIntChanged OnCapturerChanged;
	FControlAreaFloatChanged OnPercentageChanged;
	
public:	
	// Sets default values for this actor's properties
	AControlArea();

public:	
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// Pre init launch by the gameMode before the BeginPlay() function
	virtual void PreInitialisation() override;
	virtual int GetPriority() const override;

public:
	/** Zone Collide */
	virtual void initCollideElement();
	//UPROPERTY(EditAnywhere, Category = "DetectionArea")
	//	class UBoxComponent* BoxCollide;

	UPROPERTY(EditInstanceOnly, Replicated, BlueprintReadWrite, Category = "ControlAreaData")
		FString ControlAreaName = "";

	/** Control value variables */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ControlValue")
		int MaxControlValue = 20;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ControlValue")
		int MinControlValueToControl = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ControlValue")
		int ControlValueToTake = 20;

public:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRepOwner, Category = "IsTaken")
	ASoldierTeam* IsTakenBy;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRepCapturer, Category = "IsTaken")
	ASoldierTeam* IsCapturedBy;

protected:
	UPROPERTY(ReplicatedUsing = OnRepPercentage)
	float PercentageCapture = 0.f;

protected:
	UFUNCTION()
	void OnRepOwner();

	UFUNCTION()
	void OnRepCapturer();

	UFUNCTION()
	void OnRepPercentage();

	/**
	 *	Event when this actor overlaps another actor, for example a player walking into a trigger.
	 *	For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 *	Event when an actor no longer overlaps another actor, and they have separated.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:  // time value for calculation frequency
	FTimerHandle timerCalculationControlValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ControlValue")
		float timeBetweenCalcuation = 0.5;

	UFUNCTION(BlueprintCallable, Category = "ControlValue")
		void calculateControlValue();

public:
	UPROPERTY(EditInstanceOnly, Category = "ControlData")
		TMap<ASoldierTeam*, AControlAreaTeamStat*> TeamData;

protected:
	UFUNCTION(Category = "ControlData")
		void UpdateTeamData();

protected:
	UPROPERTY(EditInstanceOnly, Category = "InfluenceMap")
		int InfluenceRadius = 10;

public:

	UFUNCTION()
		int GetInfluenceRadius() const noexcept { return InfluenceRadius; }
};