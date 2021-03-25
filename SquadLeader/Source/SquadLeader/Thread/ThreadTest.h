// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HAL/Runnable.h"

/**
 * 
 */
class ThreadTest : public FRunnable
{
public:
	ThreadTest(int32 start, int32 end);
	~ThreadTest();

	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	bool IsComplete() const;

protected:

	int32 start;
	int32 end;

	bool m_stopThread;
};
