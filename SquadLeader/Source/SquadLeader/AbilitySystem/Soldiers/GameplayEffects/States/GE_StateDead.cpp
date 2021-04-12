#include "GE_StateDead.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateDead::UGE_StateDead() : UGE_State(FGameplayTag::RequestGameplayTag(FName("State.Dead")))
{
}
