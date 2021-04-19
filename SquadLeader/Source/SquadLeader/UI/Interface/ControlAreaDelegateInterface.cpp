// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaDelegateInterface.h"

// Add default functionality here for any IControlAreaDelegateInterface functions that are not pure virtual.

void IControlAreaDelegateInterface::AddControlAreaDelegate(IControlAreaInterface* newDelegate)
{
	ControlAreaDelegates.AddUnique(newDelegate);
}

void IControlAreaDelegateInterface::RemoveControlAreaDelegate(IControlAreaInterface* removeDelegate)
{
	ControlAreaDelegates.Remove(removeDelegate);
}

void IControlAreaDelegateInterface::OnControlAreaInit(unsigned int nbArea)
{
	for (auto ControlAreaInterface : ControlAreaDelegates)
	{
		ControlAreaInterface->OnControlAreaInit(nbArea);
	}
}
void IControlAreaDelegateInterface::OnAreaCaptureChanged(unsigned int index, int owner, int capturer, float capturePercent)
{
	for (auto ControlAreaInterface : ControlAreaDelegates)
	{
		ControlAreaInterface->OnAreaCaptureChanged(index, owner, capturer, capturePercent);
	}
}
