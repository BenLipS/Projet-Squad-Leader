// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityCooldownWidgiet.h"
#include "CooldownElementWidget.h"
#include "Blueprint/WidgetTree.h"

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

void UAbilityCooldownWidgiet::AddAbilityID(ESoldierAbilityInputID Key)
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

		newWidget->InitState(Key);

		CooldownBox->AddChildToHorizontalBox(newWidget);
	}
}