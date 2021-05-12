#include "GE_StateCastingSpell.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateCastingSpell::UGE_StateCastingSpell() : UGE_State(FGameplayTag::RequestGameplayTag(FName("State.CastingSpell")))
{
}