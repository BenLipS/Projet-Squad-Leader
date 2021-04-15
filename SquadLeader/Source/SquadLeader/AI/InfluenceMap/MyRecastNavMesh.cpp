// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRecastNavMesh.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "NavMesh/RecastQueryFilter.h"
#include "Runtime/Navmesh/Public/Detour/DetourNavMeshQuery.h"
#include "Runtime/Navmesh/Public/Detour/DetourNavMesh.h"
#include "Runtime/Navmesh/Public/Detour/DetourCommon.h"

const float TAB_VALUE[8]{ 12.f, 0.f, 7.f, 0.f,0.f, 16.f, 17.f, 10.f };

/*
* dtQueryFilter_SL CPP
*/
float dtQueryFilter_SL::getVirtualCost(const float* pa, const float* pb,
	const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
	const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
	const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const {
	const float Cost = GetCostInfluenceMap();
	return dtVdist(pa, pb) * Cost;
}
float dtQueryFilter_SL::GetCostInfluenceMap() const{
	auto index = FMath::RandRange(0, 7);
	return TAB_VALUE[index];
}


/*
* FRecatsQueryFilter_SL CPP
*/
FRecatsQueryFilter_SL::FRecatsQueryFilter_SL(bool bIsVirtual) : dtQueryFilter_SL(bIsVirtual){
	SetExcludedArea(RECAST_NULL_AREA);
}
INavigationQueryFilterInterface* FRecatsQueryFilter_SL::CreateCopy() const{
	return new FRecatsQueryFilter_SL(*this);
}
void FRecatsQueryFilter_SL::SetIsVirtual(bool bIsVirtual)
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter)dtQueryFilter(bIsVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL::Reset()
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter) dtQueryFilter(isVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL::SetAreaCost(uint8 AreaType, float Cost)
{
	setAreaCost(AreaType, Cost);
}
void FRecatsQueryFilter_SL::SetFixedAreaEnteringCost(uint8 AreaType, float Cost)
{
#if WITH_FIXED_AREA_ENTERING_COST
	setAreaFixedCost(AreaType, Cost);
#endif // WITH_FIXED_AREA_ENTERING_COST
}
void FRecatsQueryFilter_SL::SetExcludedArea(uint8 AreaType)
{
	setAreaCost(AreaType, DT_UNWALKABLE_POLY_COST);
}
void FRecatsQueryFilter_SL::SetAllAreaCosts(const float* CostArray, const int32 Count)
{
	// @todo could get away with memcopying to m_areaCost, but it's private and would require a little hack
	// need to consider if it's wort a try (not sure there'll be any perf gain)
	if (Count > RECAST_MAX_AREAS)
	{
		UE_LOG(LogNavigation, Warning, TEXT("FRecastQueryFilter_Example: Trying to set cost to more areas than allowed! Discarding redundant values."));
	}

	const int32 ElementsCount = FPlatformMath::Min(Count, RECAST_MAX_AREAS);
	for (int32 i = 0; i < ElementsCount; ++i)
	{
		setAreaCost(i, CostArray[i]);
	}
}
void FRecatsQueryFilter_SL::GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const
{
	const float* DetourCosts = getAllAreaCosts();
	const float* DetourFixedCosts = getAllFixedAreaCosts();

	FMemory::Memcpy(CostArray, DetourCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
	FMemory::Memcpy(FixedCostArray, DetourFixedCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
}
void FRecatsQueryFilter_SL::SetBacktrackingEnabled(const bool bBacktracking)
{
	setIsBacktracking(bBacktracking);
}
bool FRecatsQueryFilter_SL::IsBacktrackingEnabled() const
{
	return getIsBacktracking();
}
float FRecatsQueryFilter_SL::GetHeuristicScale() const { return getHeuristicScale(); }
bool FRecatsQueryFilter_SL::IsEqual(const INavigationQueryFilterInterface* Other) const
{
	// @NOTE: not type safe, should be changed when another filter type is introduced
	return FMemory::Memcmp(this, Other, sizeof(FRecastQueryFilter)) == 0;
}
void FRecatsQueryFilter_SL::SetIncludeFlags(uint16 Flags)
{

	setIncludeFlags(Flags);

}
uint16 FRecatsQueryFilter_SL::GetIncludeFlags() const
{
	return getIncludeFlags();
}
void FRecatsQueryFilter_SL::SetExcludeFlags(uint16 Flags)
{
	setExcludeFlags(Flags);
}
uint16 FRecatsQueryFilter_SL::GetExcludeFlags() const
{
	return getExcludeFlags();
}


/*
* AMyRecastNavMesh CPP
*/
AMyRecastNavMesh::AMyRecastNavMesh(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}
void AMyRecastNavMesh::BeginPlay() {
	Super::BeginPlay();
	SetupCustomNavFilter();
}
void AMyRecastNavMesh::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
void AMyRecastNavMesh::SetupCustomNavFilter() {
	if (DefaultQueryFilter.IsValid())
	{
		DefaultQueryFilter->SetFilterImplementation(dynamic_cast<const INavigationQueryFilterInterface*>(&DefaultNavFilter));
	}
}