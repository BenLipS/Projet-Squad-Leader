// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template <class interfaceClass>
class DelegateInterface
{
private:
	TArray<interfaceClass*> Delegates;

public:
	void AddDelegate(interfaceClass* newDelegate)
	{
		Delegates.AddUnique(newDelegate);
	}

	void RemoveDelegate(interfaceClass* removeDelegate)
	{
		Delegates.Remove(removeDelegate);
	}

	template<class... Args>
	void BroadCastMethod(void (interfaceClass::* method)(Args...), Args... args)
	{
		for (auto element : Delegates)
		{
			(*element.*method)(args...);
		}
	}
};