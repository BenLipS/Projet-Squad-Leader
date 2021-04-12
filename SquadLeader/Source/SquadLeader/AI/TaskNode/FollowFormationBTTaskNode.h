// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FollowFormationBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UFollowFormationBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UFollowFormationBTTaskNode();

	/* return Succeeded, Failed or InProgress */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	/*Is going to be called every Tick*/
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

	bool HysteresisDoFollow;
};
