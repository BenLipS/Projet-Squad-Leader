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

TMap<FString, FString> AGameParam::GetStringParams()
{
	TMap<FString, FString> ret;

	ret.Add(Name);

	return ret;
}
