// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOrderWheelElement.h"
#include "../../Soldiers/Players/SoldierPlayerController.h"
#include "../../Soldiers/Players/SoldierPlayer.h"

void UPlayerOrderWheelElement::OnitemSelected()
{
	//Controller call with orderEnum
	if (ASoldierPlayerController* PC = GetOwningPlayer<ASoldierPlayerController>(); PC)
	{
		if (ASoldierPlayer* Player = PC->GetPawn<ASoldierPlayer>(); Player)
		{
			PC->OnOrderGiven(Order, Player->GetLookingAtPosition());
		}
	}
}