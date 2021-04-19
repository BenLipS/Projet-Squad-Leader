// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TicketInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTicketInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ITicketInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnAllyTicketChanged(int newTicket) = 0;

	UFUNCTION()
	virtual void OnEnnemyTicketChanged(int newTicket) = 0;
};
