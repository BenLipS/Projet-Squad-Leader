// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PreInitable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPreInitable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUADLEADER_API IPreInitable
{
	GENERATED_BODY()

public:
	UFUNCTION()
		virtual void PreInitialisation() = 0;

	UFUNCTION()
		virtual int getpriority() = 0;
};
