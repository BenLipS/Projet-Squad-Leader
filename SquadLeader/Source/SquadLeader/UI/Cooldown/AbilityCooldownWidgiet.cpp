// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCooldownWidgiet.h"
#include "CooldownElementWidget.h"
#include "Blueprint/WidgetTree.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayer.h"

#include "../Interface/AbilityCooldownDelegateInterface.h"

void UAbilityCooldownWidgiet::SetupDelegateToObject_Implementation(UObject* ObjectIn)
{
	if (IAbilityCooldownDelegateInterface* AbilityCooldownDelegateInterface = Cast<IAbilityCooldownDelegateInterface>(ObjectIn); AbilityCooldownDelegateInterface)
	{
		AbilityCooldownDelegateInterface->AddAbilityCooldownDelegate(this);
	}
}

void UAbilityCooldownWidgiet::OnAbilityCooldownTriggered(float Timer, ESoldierAbilityInputID Key)
{
	for (auto Widget : CooldownBox->GetAllChildren())
	{
		if (auto CooldownElement = Cast<UCooldownElementWidget>(Widget))
		{
			if (CooldownElement->GetAbilityID() == Key)
			{
				CooldownElement->LaunchCooldown(Timer);
			}
		}
	}
}

void UAbilityCooldownWidgiet::AddAbilityID(ESoldierAbilityInputID Key, FString KeyText)
{
	for (auto Widget : CooldownBox->GetAllChildren())
	{
		if (auto CooldownElement = Cast<UCooldownElementWidget>(Widget))
		{
			if (CooldownElement->GetAbilityID() == Key)
			{
				return;
			}
		}
	}

	if (WidgetElementClass)
	{
		UCooldownElementWidget* newWidget = WidgetTree->ConstructWidget<UCooldownElementWidget>(WidgetElementClass);

		newWidget->InitState(Key, KeyText);

		CooldownBox->AddChildToHorizontalBox(newWidget);

		SoldierClass PlayerClass = SoldierClass::ASSAULT;

		if (auto MyPlayerPawn = GetOwningPlayerPawn<ASoldierPlayer>(); IsValid(MyPlayerPawn))
		{
			PlayerClass = MyPlayerPawn->GetClass();
		}

		if (CLassBrush.Contains(PlayerClass))
		{
			if (CLassBrush[PlayerClass].AbilityIcons.Contains(Key))
			{
				newWidget->SetIcon(CLassBrush[PlayerClass].AbilityIcons[Key]);
			}
		}
	}
}