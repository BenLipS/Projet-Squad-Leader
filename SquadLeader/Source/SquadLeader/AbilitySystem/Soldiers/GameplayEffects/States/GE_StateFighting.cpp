#include "GE_StateFighting.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateFighting::UGE_StateFighting() : UGE_State(FGameplayTag::RequestGameplayTag(FName("State.Fighting")), 10.f)
{
}