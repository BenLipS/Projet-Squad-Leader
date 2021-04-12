// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadTest.h"

ThreadTest::ThreadTest(int32 start, int32 end) : start{ start }, end{ end }
{

}

uint32 ThreadTest::Run() {
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(40, 2.f, FColor::Black, TEXT("Start the Thread"));

	while (!IsComplete()) {
		++start;
	}
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(50, 2.f, FColor::Red, TEXT("End of the Thread"));
	return 0;
}

void ThreadTest::Stop() {
	m_stopThread = true;
}

void ThreadTest::Exit() {

}

bool ThreadTest::IsComplete() const{
	return start >= 200000000000;
}


ThreadTest::~ThreadTest()
{
}
