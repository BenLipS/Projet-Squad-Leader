// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThreadTest.h"
#include "UObject/NoExportTypes.h"
#include "MyThreadManager.generated.h"



class FRunnableThread;
/**
 * 
 */
UCLASS()
class SQUADLEADER_API UMyThreadManager : public UObject
{
	GENERATED_BODY()
	
public:
	UMyThreadManager(const FObjectInitializer& ObjectInitializer);

	void StartProcess();

protected:
	bool IsComplete() const;

	class ThreadTest* MyThreadTest = nullptr;
	FRunnableThread* CurrentThread = nullptr;
};
