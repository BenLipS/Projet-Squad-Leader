#include "GE_StateJumping.h"
#include "../../../../Soldiers/Soldier.h"

UGE_StateJumping::UGE_StateJumping() : UGE_State(FGameplayTag::RequestGameplayTag(FName("State.Jumping")))
{
}