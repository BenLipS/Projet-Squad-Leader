#include "GC_FireWeapon.h"

UGC_FireWeapon::UGC_FireWeapon(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	GameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.FireWeapon.Instant"));
}