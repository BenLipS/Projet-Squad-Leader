#include "SL_HUD.h"
#include "SL_UserWidget.h"

#include "../Soldiers/Players/SoldierPlayerController.h"
#include "../Soldiers/Players/SoldierPlayerState.h"

#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/ControlArea/ControlAreaManager.h"
#include "SquadLeader/ControlArea/ControlArea.h"
#include "SquadLeader/SquadLeaderGameInstance.h"
#include "../Weapons/SL_Weapon.h"

void ASL_HUD::BeginPlay()
{
	Super::BeginPlay();
	for (TSubclassOf<USL_UserWidget> WidgetClass : WidgetClassList)
	{
		if (WidgetClass)
		{
			USL_UserWidget* newEntry = CreateWidget<USL_UserWidget>(GetWorld(), WidgetClass);
			if (newEntry)
			{
				newEntry->AddToViewport();
				WidgetList.Add(newEntry);
				newEntry->SetupDelegateToObject(this);
			}
		}
	}
	SetPlayerStateLink();
	SetAIStateLink();
	BindSoldierTeamChanges();
	BindControlAreas();
	//TODO : Init Tickets
}

void ASL_HUD::SetPlayerStateLink()
{
	ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController());
	if (PC)
	{
		PC->BindMainAbilities();

		if (ASoldier* Soldier = PC->GetPawn<ASoldier>(); Soldier)
		{
			if (ASL_Weapon* Weapon = Soldier->GetCurrentWeapon(); Weapon)
			{
				Weapon->SetCurrentAmmo(Weapon->GetCurrentAmmo());
				Weapon->SetMaxAmmo(Weapon->GetMaxAmmo());
			}
		}
		ASoldierPlayerState* PS = PC->GetPlayerState<ASoldierPlayerState>();
		if (PS)
		{
			//TODO faire ça plus propre ?
			PS->OnHealthChanged.RemoveAll(this);
			PS->OnMaxHealthChanged.RemoveAll(this);
			PS->OnShieldChanged.RemoveAll(this);
			PS->OnMaxShieldChanged.RemoveAll(this);
			PS->OnEXPChanged.RemoveAll(this);
			PS->OnEXPLevelUpChanged.RemoveAll(this);

			PS->OnHealthChanged.AddDynamic(this, &ASL_HUD::OnPlayerHealthChanged);
			PS->OnMaxHealthChanged.AddDynamic(this, &ASL_HUD::OnPlayerMaxHealthChanged);
			PS->OnShieldChanged.AddDynamic(this, &ASL_HUD::OnPlayerShieldChanged);
			PS->OnMaxShieldChanged.AddDynamic(this, &ASL_HUD::OnPlayerMaxShieldChanged);
			PS->OnEXPChanged.AddDynamic(this, &ASL_HUD::OnPlayerPrestigeChanged);
			PS->OnEXPLevelUpChanged.AddDynamic(this, &ASL_HUD::OnPlayerPrestigeLevelUpChanged);

			PS->BroadCastAllDatas();
		}
	}
}

void ASL_HUD::SetAIStateLink()
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController()); PC)
	{
		PC->BroadCastManagerData();
	}
}

void ASL_HUD::BindSoldierTeamChanges()
{
	if (ASquadLeaderGameState* GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS)
	{
		for (ASoldierTeam* Team : GS->GetSoldierTeamCollection())
		{
			// Get current soldiers
			for (ASoldier* Soldier : Team->GetSoldierList())
				OnSoldierAddedToTeam(Soldier);

			// Bind future SoldierTeam changes
			Team->OnSoldierAddedToList.AddDynamic(this, &ASL_HUD::OnSoldierAddedToTeam);
			Team->OnSoldierRemovedFromList.AddDynamic(this, &ASL_HUD::OnSoldierRemovedFromTeam);
		}
	}
}

void ASL_HUD::BindControlAreas()
{
	if (ASquadLeaderGameState* GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS)
	{
		// Get current control areas - TODO: Bindfuture changes
		for (AControlArea* ControlArea : GS->GetControlAreaManager()->GetControlArea())
		{
			OnControlAreaAdded(ControlArea);
		}
	}
}