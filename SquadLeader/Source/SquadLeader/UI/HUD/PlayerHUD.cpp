// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "SquadLeader/Soldiers/Players/SoldierPlayerController.h"
#include "SquadLeader/Soldiers/Players/SoldierPlayerState.h"
#include "SquadLeader/GameState/SquadLeaderGameState.h"
#include "SquadLeader/ControlArea/ControlAreaManager.h"
#include "SquadLeader/ControlArea/ControlArea.h"
#include "SquadLeader/Weapons/SL_Weapon.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
	BindControlAreas();
	BindSoldierTeamChanges();
	//TODO : Init Tickets
}

void APlayerHUD::OnInitHUD()
{
	SetPlayerStateLink();
	SetAIStateLink();
	//BindSoldierTeamChanges();
	BindControlAreas();
}

void APlayerHUD::SetPlayerStateLink()
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

			PS->OnHealthChanged.AddDynamic(this, &APlayerHUD::OnPlayerHealthChanged);
			PS->OnMaxHealthChanged.AddDynamic(this, &APlayerHUD::OnPlayerMaxHealthChanged);
			PS->OnShieldChanged.AddDynamic(this, &APlayerHUD::OnPlayerShieldChanged);
			PS->OnMaxShieldChanged.AddDynamic(this, &APlayerHUD::OnPlayerMaxShieldChanged);
			PS->OnEXPChanged.AddDynamic(this, &APlayerHUD::OnPlayerPrestigeChanged);
			PS->OnEXPLevelUpChanged.AddDynamic(this, &APlayerHUD::OnPlayerPrestigeLevelUpChanged);

			PS->BroadCastAllDatas();
		}
	}
}

void APlayerHUD::SetAIStateLink()
{
	if (ASoldierPlayerController* PC = Cast<ASoldierPlayerController>(GetOwningPlayerController()); PC)
	{
		PC->BroadCastManagerData();
	}
}

void APlayerHUD::BindSoldierTeamChanges()
{
	if (ASquadLeaderGameState* GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS)
	{
		for (ASoldierTeam* Team : GS->GetSoldierTeamCollection())
		{
			// Get current soldiers
			for (ASoldier* Soldier : Team->GetSoldierList())
				if (IsValid(Soldier))
				{
					OnSoldierAddedToTeam(Soldier);
				}

			// Bind future SoldierTeam changes
			Team->OnSoldierAddedToList.AddDynamic(this, &APlayerHUD::OnSoldierAddedToTeam);
			Team->OnSoldierRemovedFromList.AddDynamic(this, &APlayerHUD::OnSoldierRemovedFromTeam);
		}
	}
}

void APlayerHUD::BindControlAreas()
{
	if (ASquadLeaderGameState* GS = GetWorld()->GetGameState<ASquadLeaderGameState>(); GS)
	{
		// Get current control areas - TODO: Bindfuture changes
		auto ControlAreaList = GS->GetControlAreaManager()->GetControlArea();

		ControlAreaList.Sort([](const AControlArea& Left, const AControlArea& Right) {return Left.GetName() > Right.GetName(); });

		for (AControlArea* ControlArea : ControlAreaList)
		{
			OnControlAreaAdded(ControlArea);
		}
	}
}