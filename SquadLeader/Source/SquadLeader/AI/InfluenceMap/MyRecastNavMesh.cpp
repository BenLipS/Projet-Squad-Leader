// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRecastNavMesh.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "NavMesh/RecastQueryFilter.h"
#include "Runtime/Navmesh/Public/Detour/DetourNavMeshQuery.h"
#include "Runtime/Navmesh/Public/Detour/DetourNavMesh.h"
#include "Runtime/Navmesh/Public/Detour/DetourCommon.h"
/*
* dtQueryFilter_SL CPP
*/
float dtQueryFilter_SL::getVirtualCost(const float* pa, const float* pb,
	const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
	const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
	const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const {
	const float Cost = GetCostInfluenceMap(FVector2D(-pa[0], -pa[2]), FVector2D(-pb[0], -pb[2]));
	return dtVdist(pa, pb) * Cost;
}
bool dtQueryFilter_SL::passVirtualFilter(const dtPolyRef ref, const dtMeshTile* tile,	const dtPoly* poly) const{
	return passInlineFilter(ref, tile, poly);
}
float dtQueryFilter_SL::GetCostInfluenceMap(const FVector2D StartPosition, const FVector2D EndPosition) const{
	return 1.f;
}
void dtQueryFilter_SL::SetInfluenceMap(AInfluenceMapGrid* influenceMap) {
	InfluenceMap = influenceMap;
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

//////////////////////////////////////////////////////////////////////////////////


/*
* dtQueryFilter_SL CPP
*/
float dtQueryFilter_SL_TeamOne::getVirtualCost(const float* pa, const float* pb,
	const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
	const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
	const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const {
	const float Cost = GetCostInfluenceMap(FVector2D(-pa[0], -pa[2]), FVector2D(-pb[0], -pb[2]));
	return dtVdist(pa, pb) * Cost;
}
float dtQueryFilter_SL_TeamOne::GetCostInfluenceMap(const FVector2D StartPosition, const FVector2D EndPosition) const {
	float Cost = 1.f;
	if (InfluenceMap)
		Cost = InfluenceMap->GetValue(StartPosition, 2);
	return Cost;
}
void dtQueryFilter_SL_TeamOne::SetInfluenceMap(AInfluenceMapGrid* influenceMap) {
	InfluenceMap = influenceMap;
}

/*
* FRecatsQueryFilter_SL CPP
*/
FRecatsQueryFilter_SL_TeamOne::FRecatsQueryFilter_SL_TeamOne(bool bIsVirtual) : dtQueryFilter_SL_TeamOne(bIsVirtual) {
	SetExcludedArea(RECAST_NULL_AREA);
}
INavigationQueryFilterInterface* FRecatsQueryFilter_SL_TeamOne::CreateCopy() const {
	return new FRecatsQueryFilter_SL_TeamOne(*this);
}
void FRecatsQueryFilter_SL_TeamOne::SetIsVirtual(bool bIsVirtual)
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter)dtQueryFilter(bIsVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL_TeamOne::Reset()
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter) dtQueryFilter(isVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL_TeamOne::SetAreaCost(uint8 AreaType, float Cost)
{
	setAreaCost(AreaType, Cost);
}
void FRecatsQueryFilter_SL_TeamOne::SetFixedAreaEnteringCost(uint8 AreaType, float Cost)
{
#if WITH_FIXED_AREA_ENTERING_COST
	setAreaFixedCost(AreaType, Cost);
#endif // WITH_FIXED_AREA_ENTERING_COST
}
void FRecatsQueryFilter_SL_TeamOne::SetExcludedArea(uint8 AreaType)
{
	setAreaCost(AreaType, DT_UNWALKABLE_POLY_COST);
}
void FRecatsQueryFilter_SL_TeamOne::SetAllAreaCosts(const float* CostArray, const int32 Count)
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
void FRecatsQueryFilter_SL_TeamOne::GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const
{
	const float* DetourCosts = getAllAreaCosts();
	const float* DetourFixedCosts = getAllFixedAreaCosts();

	FMemory::Memcpy(CostArray, DetourCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
	FMemory::Memcpy(FixedCostArray, DetourFixedCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
}
void FRecatsQueryFilter_SL_TeamOne::SetBacktrackingEnabled(const bool bBacktracking)
{
	setIsBacktracking(bBacktracking);
}
bool FRecatsQueryFilter_SL_TeamOne::IsBacktrackingEnabled() const
{
	return getIsBacktracking();
}
float FRecatsQueryFilter_SL_TeamOne::GetHeuristicScale() const { return getHeuristicScale(); }
bool FRecatsQueryFilter_SL_TeamOne::IsEqual(const INavigationQueryFilterInterface* Other) const
{
	// @NOTE: not type safe, should be changed when another filter type is introduced
	return FMemory::Memcmp(this, Other, sizeof(FRecastQueryFilter)) == 0;
}
void FRecatsQueryFilter_SL_TeamOne::SetIncludeFlags(uint16 Flags)
{

	setIncludeFlags(Flags);

}
uint16 FRecatsQueryFilter_SL_TeamOne::GetIncludeFlags() const
{
	return getIncludeFlags();
}
void FRecatsQueryFilter_SL_TeamOne::SetExcludeFlags(uint16 Flags)
{
	setExcludeFlags(Flags);
}
uint16 FRecatsQueryFilter_SL_TeamOne::GetExcludeFlags() const
{
	return getExcludeFlags();
}

//////////////////////////////////////////////////////////////////////////////////

/*
* dtQueryFilter_SL CPP
*/
float dtQueryFilter_SL_TeamTwo::getVirtualCost(const float* pa, const float* pb,
	const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
	const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
	const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const {
	const float Cost = GetCostInfluenceMap(FVector2D(-pa[0], -pa[2]), FVector2D(-pb[0], -pb[2]));
	return dtVdist(pa, pb) * Cost;
}
float dtQueryFilter_SL_TeamTwo::GetCostInfluenceMap(const FVector2D StartPosition, const FVector2D EndPosition) const {
	float Cost = 1.f;
	if (InfluenceMap)
		Cost = InfluenceMap->GetValue(StartPosition, 1);
	return Cost;
}
void dtQueryFilter_SL_TeamTwo::SetInfluenceMap(AInfluenceMapGrid* influenceMap) {
	InfluenceMap = influenceMap;
}

/*
* FRecatsQueryFilter_SL CPP
*/
FRecatsQueryFilter_SL_TeamTwo::FRecatsQueryFilter_SL_TeamTwo(bool bIsVirtual) : dtQueryFilter_SL_TeamTwo(bIsVirtual) {
	SetExcludedArea(RECAST_NULL_AREA);
}
INavigationQueryFilterInterface* FRecatsQueryFilter_SL_TeamTwo::CreateCopy() const {
	return new FRecatsQueryFilter_SL_TeamTwo(*this);
}
void FRecatsQueryFilter_SL_TeamTwo::SetIsVirtual(bool bIsVirtual)
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter)dtQueryFilter(bIsVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL_TeamTwo::Reset()
{
	dtQueryFilter* Filter = static_cast<dtQueryFilter*>(this);
	Filter = new(Filter) dtQueryFilter(isVirtual);
	SetExcludedArea(RECAST_NULL_AREA);
}
void FRecatsQueryFilter_SL_TeamTwo::SetAreaCost(uint8 AreaType, float Cost)
{
	setAreaCost(AreaType, Cost);
}
void FRecatsQueryFilter_SL_TeamTwo::SetFixedAreaEnteringCost(uint8 AreaType, float Cost)
{
#if WITH_FIXED_AREA_ENTERING_COST
	setAreaFixedCost(AreaType, Cost);
#endif // WITH_FIXED_AREA_ENTERING_COST
}
void FRecatsQueryFilter_SL_TeamTwo::SetExcludedArea(uint8 AreaType)
{
	setAreaCost(AreaType, DT_UNWALKABLE_POLY_COST);
}
void FRecatsQueryFilter_SL_TeamTwo::SetAllAreaCosts(const float* CostArray, const int32 Count)
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
void FRecatsQueryFilter_SL_TeamTwo::GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const
{
	const float* DetourCosts = getAllAreaCosts();
	const float* DetourFixedCosts = getAllFixedAreaCosts();

	FMemory::Memcpy(CostArray, DetourCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
	FMemory::Memcpy(FixedCostArray, DetourFixedCosts, sizeof(float) * FMath::Min(Count, RECAST_MAX_AREAS));
}
void FRecatsQueryFilter_SL_TeamTwo::SetBacktrackingEnabled(const bool bBacktracking)
{
	setIsBacktracking(bBacktracking);
}
bool FRecatsQueryFilter_SL_TeamTwo::IsBacktrackingEnabled() const
{
	return getIsBacktracking();
}
float FRecatsQueryFilter_SL_TeamTwo::GetHeuristicScale() const { return getHeuristicScale(); }
bool FRecatsQueryFilter_SL_TeamTwo::IsEqual(const INavigationQueryFilterInterface* Other) const
{
	// @NOTE: not type safe, should be changed when another filter type is introduced
	return FMemory::Memcmp(this, Other, sizeof(FRecastQueryFilter)) == 0;
}
void FRecatsQueryFilter_SL_TeamTwo::SetIncludeFlags(uint16 Flags)
{

	setIncludeFlags(Flags);

}
uint16 FRecatsQueryFilter_SL_TeamTwo::GetIncludeFlags() const
{
	return getIncludeFlags();
}
void FRecatsQueryFilter_SL_TeamTwo::SetExcludeFlags(uint16 Flags)
{
	setExcludeFlags(Flags);
}
uint16 FRecatsQueryFilter_SL_TeamTwo::GetExcludeFlags() const
{
	return getExcludeFlags();
}

//////////////////////////////////////////////////////////////////////////////////


/*
* AMyRecastNavMesh CPP
*/
AMyRecastNavMesh::AMyRecastNavMesh(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultNavFilter = new FRecatsQueryFilter_SL{};
	TeamOneNavFilter = new FRecatsQueryFilter_SL_TeamOne{};
	TeamTwoNavFilter = new FRecatsQueryFilter_SL_TeamTwo{};
}
void AMyRecastNavMesh::BeginPlay() {
	Super::BeginPlay();
	SetupCustomNavFilter();
}
void AMyRecastNavMesh::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (!InfluenceMap)
		InitInfluenceMap();
}
void AMyRecastNavMesh::SetupCustomNavFilter() {
	if (DefaultQueryFilter.IsValid())
	{
		DefaultQueryFilter->SetFilterImplementation(dynamic_cast<const INavigationQueryFilterInterface*>(DefaultNavFilter));
	}
}
void AMyRecastNavMesh::InitInfluenceMap(){
	TArray<AActor*> ActorList = GetLevel()->Actors;
	auto functor = [](AActor* actor) { return actor && actor->GetName() == "BP_InfluenceMap_C_0"; };
	AActor* influenceMap = *(ActorList.FindByPredicate(functor));
	if (influenceMap) {
		InfluenceMap = Cast<AInfluenceMapGrid>(influenceMap);
		DefaultNavFilter->SetInfluenceMap(InfluenceMap);
		TeamOneNavFilter->SetInfluenceMap(InfluenceMap);
		TeamTwoNavFilter->SetInfluenceMap(InfluenceMap);
		SetupCustomNavFilter();

		FNavigationQueryFilter* Filter1 = new FNavigationQueryFilter();
		Filter1->SetFilterImplementation(dynamic_cast<const INavigationQueryFilterInterface*>(TeamOneNavFilter));
		StoreQueryFilter(FilterTeam1, MakeShareable(Filter1));

		FNavigationQueryFilter* Filter2 = new FNavigationQueryFilter();
		Filter2->SetFilterImplementation(dynamic_cast<const INavigationQueryFilterInterface*>(TeamTwoNavFilter));
		StoreQueryFilter(FilterTeam2, MakeShareable(Filter2));
	}
}