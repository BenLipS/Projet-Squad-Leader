#include "ControlArea.h"
#include "../GameState/SquadLeaderGameState.h"
#include "../SquadLeaderGameModeBase.h"
#include "../Soldiers/Soldier.h"
#include "../Soldiers/Players/SoldierPlayerController.h"
#include "../UI/SL_HUD.h"
#include "ControlAreaManager.h"


// Sets default values
AControlArea::AControlArea()
{
	initCollideElement();

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// used when initialising the control area
void AControlArea::initCollideElement() {
}

void AControlArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AControlArea, IsTakenBy, COND_None, REPNOTIFY_Always);
}


void AControlArea::PreInitialisation()
{
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
		// add this to the game mode collection
		auto test = GS->GetControlAreaManager();
		test->AddControlArea(this);

		UpdateTeamData();
	}
}

int AControlArea::GetPriority() const
{
	return 2;
}


void AControlArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (GetLocalRole() == ROLE_Authority) 
	{  // server only
		if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
			if (TeamData.Contains(soldier->GetTeam())) {
				TeamData[soldier->GetTeam()]->presenceTeam++;

				// initiate the calculation of the control zone value if needed
				if (!timerCalculationControlValue.IsValid())
					GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
						&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
			}
			else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("ControlArea : Player of an unknow team"));
		}
	}
}

void AControlArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (GetLocalRole() == ROLE_Authority) {  // server only
		if (ASoldier* soldier = Cast<ASoldier>(OtherActor); soldier) {
			if (TeamData.Contains(soldier->GetTeam())) {
				if (TeamData[soldier->GetTeam()]->presenceTeam > 0) {
					TeamData[soldier->GetTeam()]->presenceTeam--;
				}

				// begin the calculation if everybody of this team left and the calculation is not already working
				if (TeamData[soldier->GetTeam()]->presenceTeam == 0) {
					if (!timerCalculationControlValue.IsValid())
						GetWorldTimerManager().SetTimer(timerCalculationControlValue, this,
							&AControlArea::calculateControlValue, timeBetweenCalcuation, true, timeBetweenCalcuation);
				}
			}
		}
	}
}

void AControlArea::calculateControlValue()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : None"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Max value reach"));*/
	if (GetLocalRole() == ROLE_Authority) {  // only for the server

		// check info about the differents teams on point
		int nbTeamOnPoint = 0;
		ASoldierTeam* presentTeam = nullptr;
		for (TPair<ASoldierTeam*, AControlAreaTeamStat*> team : TeamData) {
			if (team.Value->presenceTeam > 0) {
				nbTeamOnPoint++;
				presentTeam = team.Key;
			}
		}

		// handling teams information
		if (presentTeam && nbTeamOnPoint == 1) {
			if (TeamData[presentTeam]->controlValue < MaxControlValue) {
				bool needToDecreaseOtherPresenceFirst = false;
				for (auto& otherTeam : TeamData) {  // reduce the control value in each other team by the number of teamate
					if (otherTeam.Key != presentTeam) {
						if (otherTeam.Value->controlValue >= TeamData[presentTeam]->presenceTeam) {
							otherTeam.Value->controlValue -= TeamData[presentTeam]->presenceTeam;
							needToDecreaseOtherPresenceFirst = true;
						}
						else {
							otherTeam.Value->controlValue = 0;
						}
						if (IsTakenBy == otherTeam.Key && otherTeam.Value->controlValue < MinControlValueToControl) {  // remove IsTakenBy if needed
							// notify here the changement if needed
							IsTakenBy = nullptr;
							otherTeam.Value->ChangeSpawnState(false);
							GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control = None"));
						}
						ClientNotifyValueChange(otherTeam.Value->controlValue, IsTakenBy, otherTeam.Key);  // call client function to notify the modification
					}
				}
				if (!needToDecreaseOtherPresenceFirst) {  // if all other presence value is 0
					if (TeamData[presentTeam]->controlValue + TeamData[presentTeam]->presenceTeam < MaxControlValue)
						TeamData[presentTeam]->controlValue += TeamData[presentTeam]->presenceTeam;
					else
						TeamData[presentTeam]->controlValue = MaxControlValue;

					if (IsTakenBy != presentTeam && TeamData[presentTeam]->controlValue >= ControlValueToTake) {  // take control of the point
						IsTakenBy = presentTeam;
						TeamData[presentTeam]->ChangeSpawnState(true);
						// notify here the changement if needed
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Team control =" + presentTeam->TeamName));

						if (ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); GameMode) {
							GameMode->NotifyControlAreaCaptured(this);
						}
					}
					ClientNotifyValueChange(TeamData[presentTeam]->controlValue, IsTakenBy, presentTeam);  // call client function to notify the modification
				}
			}
			else { // stop the timer
				GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea : Max control for " + presentTeam->TeamName));

				auto GM = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode());
				FGridPackage m_package;
				m_package.m_location_on_map = GetActorLocation();
				switch (presentTeam->Id) {
				case 1:
					m_package.team_value = 1;
					break;
				case 2:
					m_package.team_value = 2;
					break;
				default:
					break;
				}
				m_package.m_type = Type::ControlArea;
				GM->InfluenceMap->ReceivedMessage(m_package);
			}
		}
		else {  // too much teams on points or nobody
			if (nbTeamOnPoint == 0) {
			}
			// stop the timer
			GetWorld()->GetTimerManager().ClearTimer(timerCalculationControlValue);
		}
	}
}

void AControlArea::UpdateTeamData()
{
	if (GetLocalRole() == ROLE_Authority) {  // only for the server
		if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
			auto teamCollection = GS->GetSoldierTeamCollection();

			TArray<ASoldierTeam*> keyToRemove;  // remove element
			for (TPair<ASoldierTeam*, AControlAreaTeamStat*> team : TeamData) {
				if (!teamCollection.Contains(team.Key)) {
					keyToRemove.Add(team.Key);
					//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea Update : Unknown team removed : " + team.Key.GetDefaultObject()->TeamName));
				}
			}
			for (auto key : keyToRemove) {
				TeamData.Remove(key);
			}

			for (auto team : teamCollection) {  // add element
				if (!TeamData.Contains(team)) {
					TeamData.Add(team, NewObject<AControlAreaTeamStat>());
					//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("ControlArea Update : Unknown team added : " + team.GetDefaultObject()->TeamName));
				}
			}

			for (auto& team : TeamData) {  // update spawn state
				if (team.Value) {
					team.Value->ChangeSpawnState(IsTakenBy == team.Key);
					team.Value->ChangeSpawnTeam(team.Key);
				}
			}
		}
	}
}

void AControlArea::ClientNotifyValueChange(int Value, ASoldierTeam* ControlAreaOwner, ASoldierTeam* ControlAreaMaster)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, TEXT("ControlArea : client function"), true);
	if (auto GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS) {
		int index = GS->GetControlAreaManager()->ControlAreaIndex(this);
		if (index != -1) {  // only if the control area is in the list
			ASoldierPlayerController* playerController = GetWorld()->GetFirstPlayerController<ASoldierPlayerController>();
			if (playerController)
			{
				ASoldierTeam* PlayerTeam = playerController->GetTeam();
			
				float Percent = (Value + 0.0) / MaxControlValue;
				int AreaOwner = 0;
				if (ControlAreaOwner) {
					if (ControlAreaOwner == PlayerTeam) {
						AreaOwner = 1;
					}
					else AreaOwner = -1;
				}
				int AreaCapturer = 0;
				if (ControlAreaMaster) {
					if (ControlAreaMaster == PlayerTeam) {
						AreaCapturer = 1;
					}
					else AreaCapturer = -1;
				}
				OnControlAreaChanged.Broadcast(AreaOwner, AreaCapturer, Percent);
			}
		}
	}
}