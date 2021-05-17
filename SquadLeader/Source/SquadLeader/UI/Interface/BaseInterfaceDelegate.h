// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseInterfaceDelegate.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBaseInterfaceDelegate : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IBaseInterfaceDelegate
{
	GENERATED_BODY()

protected:

	//used to apply a member function of T to all T in a TArray
	template<class T, class... Args>
	static void BroadCastMethod(void (T::* method)(Args...), const TArray<T*>& delegateContainer, Args... args)
	{
		for (auto delegate : delegateContainer)
		{
			(*delegate.*method)(args...);
		}
	}
};
