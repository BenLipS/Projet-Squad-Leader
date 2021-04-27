#include "GameParam.h"
#include "Kismet/KismetMathLibrary.h"

void UGameParam::RandomiseParam(UGameParam* MinConfig, UGameParam* MaxConfig)
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
