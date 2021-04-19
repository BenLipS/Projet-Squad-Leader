// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TicketInterface.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TicketDelegateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTicketDelegateInterface : public UTicketInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API ITicketDelegateInterface : public ITicketInterface
{
	GENERATED_BODY()
private:
	TArray<ITicketInterface*> TicketDelegates;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnAllyTicketChanged(int newTicket) override;
	virtual void OnEnnemyTicketChanged(int newTicket) override;

	void AddTicketDelegate(ITicketInterface* newDelegate);
	void RemoveTicketDelegate(ITicketInterface* removeDelegate);
};
