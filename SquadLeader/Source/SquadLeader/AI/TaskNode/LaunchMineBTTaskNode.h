// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LaunchMineBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ULaunchMineBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	ULaunchMineBTTaskNode();

	/*Is going to be called the first time we run the node (he can return Succeded, Faile or InProgressed)*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;
	
};
