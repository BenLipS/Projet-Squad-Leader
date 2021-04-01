#include "GE_StateFiring.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateFiring::UGE_StateFiring() : UGE_State(FGameplayTag::RequestGameplayTag(FName("State.Firing")), 2.f)
{
}