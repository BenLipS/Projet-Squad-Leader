// Copyright 2020 Dan Kestranek.

#include "SL_GameplayEffectTypes.h"

bool FSL_GameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	return Super::NetSerialize(Ar, Map, bOutSuccess) && TargetData.NetSerialize(Ar, Map, bOutSuccess);
}