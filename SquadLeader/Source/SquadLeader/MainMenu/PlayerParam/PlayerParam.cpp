#include "PlayerParam.h"

APlayerParam::APlayerParam()
{
	bReplicates = true;
}

void APlayerParam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

TSubclassOf<ASoldierAI> APlayerParam::GetAIClass(int AINb)
{
	if (AINb < AIClass.Num()) {
		return AIClass[AINb];
	}
	return AIClass[0];
}

void APlayerParam::SetAIClass(TSubclassOf<ASoldierAI> NewClass, int AINb)
{
	if (AINb < AIClass.Num()) {
		AIClass[AINb] = NewClass;
	}
	else if (AINb == AIClass.Num()) {
		AIClass.Add(NewClass);
	}
}

void APlayerParam::AdaptAllAIToTeam()
{
	for (int loop = 0; loop < AIClass.Num(); loop++) {
		AdaptAIToTeam(loop);
	}
}

void APlayerParam::AdaptAIToTeam(int AINb)
{
	if (AINb < AIClass.Num() && (TeamID == 1 || TeamID == 2)) {
		if (TeamID == 1) {
			if (auto index = ReferenceClassAITeam2.Find(AIClass[AINb]); index != INDEX_NONE) {
				// we find the given archetype in the list of Team2 AI reference. We need to take the same element in Team1
				AIClass[AINb] = ReferenceClassAITeam1[index];
			}
		}
		else {
			if (auto index = ReferenceClassAITeam1.Find(AIClass[AINb]); index != INDEX_NONE) {
				// we find the given archetype in the list of Team1 AI reference. We need to take the same element in Team2
				AIClass[AINb] = ReferenceClassAITeam2[index];
			}
		}
	}
}