#include "PlayerParam.h"

APlayerParam::APlayerParam()
{
	bReplicates = true;
}

void APlayerParam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

const TSubclassOf<ASoldierPlayer> APlayerParam::GetPlayerSoldier()
{
	return *ReferenceSoldier.Find(PlayerClass);
}

const SoldierClass APlayerParam::GetAIClass(int AINb)
{
	if (AINb < AIClass.Num()) {
		return AIClass[AINb];
	}
	return AIClass[0];
}

void APlayerParam::SetAIClass(SoldierClass NewClass, int AINb)
{
	if (AINb < AIClass.Num()) {
		AIClass[AINb] = NewClass;
	}
	else if (AINb == AIClass.Num()) {
		AIClass.Add(NewClass);
	}
}

const TSubclassOf<ASoldierAI> APlayerParam::GetAISoldier(int AINb)
{
	if (TeamID == 1)
		return *ReferenceClassAITeam1.Find(GetAIClass(AINb));
	else
		return *ReferenceClassAITeam2.Find(GetAIClass(AINb));
}

