#include "PrestigeWidget.h"
#include "SquadLeader/Soldiers/Soldier.h"

UPrestigeWidget::UPrestigeWidget(const FObjectInitializer& ObjectInitializer) : USL_UserWidget(ObjectInitializer)
{
}

void UPrestigeWidget::SetupDelegateToObject(UObject* ObjectIn)
{
	if (IPlayerPrestigeDelegateInterface* PlayerPrestigeDelegateInterface = Cast<IPlayerPrestigeDelegateInterface>(ObjectIn); PlayerPrestigeDelegateInterface)
	{
		PlayerPrestigeDelegateInterface->AddPlayerPrestigeDelegate(this);
	}
}

// Note: EXP == Prestige - EXP = Dev name | Prestige = Player name
void UPrestigeWidget::OnPlayerPrestigeChanged(float newValue)
{
	Prestige = newValue;

	if (ASoldier* Soldier = Cast<ASoldier>(GetOwningPlayerPawn()); Soldier && Soldier->GetAttributeSet())
	{
		const float AccumulatedPrestige = Soldier->GetAttributeSet()->GetEXPAccumulated();
		ProgressBarPrestige->SetPercent((Prestige - AccumulatedPrestige) / (PrestigeLevelUp - AccumulatedPrestige));
		//TextPrestige->SetText(FText::FromString(FString::SanitizeFloat(Prestige, 0.0f) + "/" + FString::SanitizeFloat(PrestigeLevelUp, 0.0f)));
	}
	else
		ProgressBarPrestige->SetPercent(0.f);
}

// Note: EXP == Prestige - EXP = Dev name | Prestige = Player name
void UPrestigeWidget::OnPlayerPrestigeLevelUpChanged(float newValue)
{
	PrestigeLevelUp = newValue;

	if (ASoldier* Soldier = Cast<ASoldier>(GetOwningPlayerPawn()); Soldier && Soldier->GetAttributeSet())
	{
		const float AccumulatedPrestige = Soldier->GetAttributeSet()->GetEXPAccumulated();
		ProgressBarPrestige->SetPercent((Prestige - AccumulatedPrestige) / (PrestigeLevelUp - AccumulatedPrestige));
		//TextPrestige->SetText(FText::FromString(FString::SanitizeFloat(Prestige, 0.0f) + "/" + FString::SanitizeFloat(PrestigeLevelUp, 0.0f)));
	}
	else
		ProgressBarPrestige->SetPercent(0.f);
}