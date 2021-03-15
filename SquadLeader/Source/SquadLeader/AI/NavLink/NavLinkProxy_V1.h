// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkProxy_V1.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API ANavLinkProxy_V1 : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:

	ANavLinkProxy_V1();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Jump(AActor* Agent, const FVector& Destination);
};
