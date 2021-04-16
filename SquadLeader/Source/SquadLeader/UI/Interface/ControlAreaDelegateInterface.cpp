// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaDelegateInterface.h"

// Add default functionality here for any IControlAreaDelegateInterface functions that are not pure virtual.

void IControlAreaDelegateInterface::AddControlAreaDelegate(IControlAreaInterface* newDelegate)
{
}

void IControlAreaDelegateInterface::RemoveControlAreaDelegate(IControlAreaInterface* removeDelegate)
{
}

void IControlAreaDelegateInterface::OnControlAreaInit(unsigned int nbArea)
{

}
void IControlAreaDelegateInterface::OnAreaCaptureChanged(unsigned int index, int owner, float capturePercent)
{
	
}
