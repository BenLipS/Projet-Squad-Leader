// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "../../WorldObject/Wall.h"
#include "GridBase.generated.h"

UCLASS()
class SQUADLEADER_API AGridBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
		float GetValue() const noexcept { return m_value; }


protected:
	UPROPERTY(BlueprintReadOnly, Category = "value")
		float m_value;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Trigger box")
		class UBoxComponent* TriggerBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** called when something enters the sphere component */
	UFUNCTION()
		virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
		virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

};
