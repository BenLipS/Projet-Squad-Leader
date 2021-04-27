// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FinishStunBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UFinishStunBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UFinishStunBTTaskNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;
};
