#include "ControlAreaManager.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"
#include "SquadLeader/UI/HUD/SL_HUD.h"
#include "../Soldiers/Players/SoldierPlayerController.h"

AControlAreaManager::AControlAreaManager()
{
	bReplicates = true;
}

TArray<AControlArea*> AControlAreaManager::GetControlAreaList() const
{
	return ControlAreaList;
}

void AControlAreaManager::PreInitialisation()
{
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
		GS->SetControlAreaManager(this);
	}
}

int AControlAreaManager::GetPriority() const
{
	return 1;
}

void AControlAreaManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AControlAreaManager, ControlAreaList, COND_None, REPNOTIFY_Always);
}


void AControlAreaManager::AddControlArea(AControlArea* _NewArea)
{
	ControlAreaList.AddUnique(_NewArea);
}

void AControlAreaManager::CleanControlAreaList()
{
	ControlAreaList.RemoveAll([](AControlArea* element) {return element == nullptr; });
}


TArray<AControlArea*> AControlAreaManager::GetAreaControlledByTeam(ASoldierTeam* _Team)
{
	TArray<AControlArea*> selection;
	for (auto element : ControlAreaList) {
		if (element->GetIsTakenBy() == _Team) {
			selection.Add(element);
		}
	}
	return selection;
}


ASoldierTeam* AControlAreaManager::GetTeamWithControlAdvantage()  // to try with UI
{
	if (GetLocalRole() == ROLE_Authority) {  // only for the server
		if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
			int nbControleAreaToObtain = ControlAreaList.Num() - (ControlAreaList.Num() / 2);
			for (ASoldierTeam* team : GS->GetSoldierTeamCollection()) {
				if (GetAreaControlledByTeam(team).Num() >= nbControleAreaToObtain) {
					return team;
				}
			}
		}
	}
	return nullptr;
}

ASoldierTeam* AControlAreaManager::GetTeamWithAllControl()
{
	if (ControlAreaList.Num() > 0) {
		for (auto element : ControlAreaList) {
			if (element->GetIsTakenBy() != ControlAreaList[0]->GetIsTakenBy()) {
				return nullptr;
			}
		}
		return ControlAreaList[0]->GetIsTakenBy();
	}
	return nullptr;
}

int AControlAreaManager::ControlAreaIndex(AControlArea* Element)
{
	int index = 0;
	bool result = ControlAreaList.Find(Element, index);
	if (result) {
		return index;
	}
	return -1;
}