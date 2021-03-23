// Fill out your copyright notice in the Description page of Project Settings.

#include "MyThreadManager.h"
#include "HAL/RunnableThread.h"

UMyThreadManager::UMyThreadManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyThreadManager::StartProcess() {
	if (!CurrentThread && FPlatformProcess::SupportsMultithreading()) {
		MyThreadTest = new ThreadTest(1,200);
		CurrentThread = FRunnableThread::Create(MyThreadTest, TEXT("FIRST THREAD EVER WITH UNREAL"));
	}
}

bool UMyThreadManager::IsComplete() const{
	return !CurrentThread || MyThreadTest->IsComplete();
}