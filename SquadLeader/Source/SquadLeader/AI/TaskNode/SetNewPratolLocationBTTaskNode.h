// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SetNewPratolLocationBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API USetNewPratolLocationBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USetNewPratolLocationBTTaskNode();
	
	/* return Succeeded, Failed or InProgress */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	virtual FString GetStaticDescription() const override;

};
