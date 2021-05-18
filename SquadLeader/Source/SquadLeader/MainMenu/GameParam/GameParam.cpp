#include "GameParam.h"
#include "Kismet/KismetMathLibrary.h"

AGameParam::AGameParam()
{
	bReplicates = true;
}

void AGameParam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, Name, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, LevelTarget, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, LevelRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, NbAIBasicAssault, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, NbAIBasicHeavy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, LevelAIBasic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, StartingNbAISquad, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, LevelAISquad, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, NbTickets, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, RespawnDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, Weather, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGameParam, FriendOnly, COND_None, REPNOTIFY_Always);
}

void AGameParam::RandomiseParam(AGameParam* MinConfig, AGameParam* MaxConfig)
{
	NbAIBasicAssault = UKismetMathLibrary::RandomIntegerInRange(MinConfig->NbAIBasicAssault, MaxConfig->NbAIBasicAssault);
	NbAIBasicHeavy = UKismetMathLibrary::RandomIntegerInRange(MinConfig->NbAIBasicHeavy, MaxConfig->NbAIBasicHeavy);
	LevelAIBasic = UKismetMathLibrary::RandomIntegerInRange(MinConfig->LevelAIBasic, MaxConfig->LevelAIBasic);
	StartingNbAISquad = UKismetMathLibrary::RandomIntegerInRange(MinConfig->StartingNbAISquad, MaxConfig->StartingNbAISquad);
	LevelAISquad = UKismetMathLibrary::RandomIntegerInRange(MinConfig->LevelAISquad, MaxConfig->LevelAISquad);
	NbTickets = UKismetMathLibrary::RandomIntegerInRange(MinConfig->NbTickets, MaxConfig->NbTickets);
	Weather = UKismetMathLibrary::RandomIntegerInRange(MinConfig->Weather, MaxConfig->Weather);
	RespawnDuration = UKismetMathLibrary::RandomIntegerInRange(MinConfig->RespawnDuration, MaxConfig->RespawnDuration);
}

void AGameParam::GameParamCopy(AGameParam* TargetParam)
{
	NbAIBasicAssault = TargetParam->NbAIBasicAssault;
	NbAIBasicHeavy = TargetParam->NbAIBasicHeavy;
	LevelAIBasic = TargetParam->LevelAIBasic;
	StartingNbAISquad = TargetParam->StartingNbAISquad;
	LevelAISquad = TargetParam->LevelAISquad;
	NbTickets = TargetParam->NbTickets;
	Weather = TargetParam->Weather;
	RespawnDuration = TargetParam->RespawnDuration;
}

bool AGameParam::IntParamExist(FString ParamName)
{
	auto paramList = GetIntParams();
	if (paramList.Contains(ParamName))
	{
		return true;
	}
	return false;
}

int AGameParam::GetIntParam(FString ParamName)
{
	auto paramList = GetIntParams();
	if (paramList.Contains(ParamName))
	{
		return paramList[ParamName];
	}

	return -1;
}

TMap<FString, int> AGameParam::GetIntParams()
{
	TMap<FString, int> ret;

	ret.Add("LevelTarget", LevelTarget);
	ret.Add("LevelRange", LevelRange);
	ret.Add("NbAIBasicAssault", NbAIBasicAssault);
	ret.Add("NbAIBasicHeavy", NbAIBasicHeavy);
	ret.Add("LevelAIBasic", LevelAIBasic);
	ret.Add("StartingNbAISquad", StartingNbAISquad);
	ret.Add("LevelAISquad", LevelAISquad);
	ret.Add("NbTickets", NbTickets);
	ret.Add("Weather", Weather);
	ret.Add("RespawnDuration", RespawnDuration);

	return ret;
}

void AGameParam::SetIntParams(TMap<FString, int> ImportedParam)
{
	if(ImportedParam.Contains("LevelTarget")) LevelTarget = ImportedParam["LevelTarget"];
	if(ImportedParam.Contains("LevelRange")) LevelRange = ImportedParam["LevelRange"];
	if(ImportedParam.Contains("NbAIBasicAssault")) NbAIBasicAssault = ImportedParam["NbAIBasicAssault"];
	if(ImportedParam.Contains("NbAIBasicHeavy")) NbAIBasicHeavy = ImportedParam["NbAIBasicHeavy"];
	if(ImportedParam.Contains("LevelAIBasic")) LevelAIBasic = ImportedParam["LevelAIBasic"];
	if(ImportedParam.Contains("StartingNbAISquad")) StartingNbAISquad = ImportedParam["StartingNbAISquad"];
	if(ImportedParam.Contains("LevelAISquad")) LevelAISquad = ImportedParam["LevelAISquad"];
	if(ImportedParam.Contains("NbTickets")) NbTickets = ImportedParam["NbTickets"];
	if(ImportedParam.Contains("Weather")) Weather = ImportedParam["Weather"];
	if(ImportedParam.Contains("RespawnDuration")) RespawnDuration = ImportedParam["RespawnDuration"];
}

TMap<FString, FString> AGameParam::GetStringParams()
{
	TMap<FString, FString> ret;

	ret.Add("Name", Name);

	return ret;
}

void AGameParam::SetStringParams(TMap<FString, FString> ImportedParam)
{
	if (ImportedParam.Contains("Name")) Name = ImportedParam["Name"];
}
