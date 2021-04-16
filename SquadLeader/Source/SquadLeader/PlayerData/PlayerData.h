// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SQUADLEADER_API PlayerData
{
public:
	PlayerData();
	~PlayerData();

public:  // data for players
	FString Id;
	FString GuestToken;
	FString Name;
	FString IpAdress;
	int NbKillIA;
	int NbKillPlayer;
	int NbDeathIA;
	int NbDeathPlayer;
	int NbVictory;
	int NbLoss;
	int Score;
	int PlayTime;
	TArray<FString> Friend;
	
	void LoadOrCreate(FString FileName);

protected:  // utilitarian functions
	FString ReadFile(FString FileName);
	void WriteFile(FString FileName, FString Content);
	void ImportDataFromJson(FString Content);
	FString ExportDataInJson();
};
