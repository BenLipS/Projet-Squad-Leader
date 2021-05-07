// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuListGameItemWidget.h"

void UMenuListGameItemWidget::SetIPAdress(FString AdressIn)
{
	IPAdress = AdressIn;
}

FString UMenuListGameItemWidget::GetIPAdress()
{
	return IPAdress;
}
