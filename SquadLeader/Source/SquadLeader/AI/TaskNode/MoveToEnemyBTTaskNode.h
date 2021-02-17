// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToEnemyBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMoveToEnemyBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UMoveToEnemyBTTaskNode();

	/*Is going to be called the first time we run the node (he can return Succeded, Faile or InProgressed)*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	/*Is going to be called every Tick*/
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual FString GetStaticDescription() const override;

};
