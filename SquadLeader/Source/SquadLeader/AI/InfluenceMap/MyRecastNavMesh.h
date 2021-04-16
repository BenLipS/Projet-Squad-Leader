// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Detour/DetourNavMeshQuery.h"
#include "NavMesh/RecastNavMesh.h"
#include "InfluenceMapGrid.h"
#include "MyRecastNavMesh.generated.h"

/*
* Default Filter
*/
class SQUADLEADER_API dtQueryFilter_SL : public dtQueryFilter {
public:
	dtQueryFilter_SL(bool inIsVirtual = true) : dtQueryFilter(inIsVirtual){}

	virtual ~dtQueryFilter_SL(){}

protected:
	virtual float getVirtualCost(const float* pa, const float* pb,
		const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
		const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
		const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const override;

	virtual bool passVirtualFilter(const dtPolyRef ref,
		const dtMeshTile* tile,
		const dtPoly* poly) const override;
private:
	float GetCostInfluenceMap(const FVector2D StartPosition, const FVector2D EndPosition) const;

public:
	AInfluenceMapGrid* InfluenceMap;
	void SetInfluenceMap(AInfluenceMapGrid* influenceMap);
};
class SQUADLEADER_API FRecatsQueryFilter_SL : public INavigationQueryFilterInterface, public dtQueryFilter_SL {
public:
	FRecatsQueryFilter_SL(bool bIsVirtual = true);
	~FRecatsQueryFilter_SL(){}

	virtual void Reset() override;

	virtual void SetAreaCost(uint8 AreaType, float Cost) override;
	virtual void SetFixedAreaEnteringCost(uint8 AreaType, float Cost) override;
	virtual void SetExcludedArea(uint8 AreaType) override;
	virtual void SetAllAreaCosts(const float* CostArray, const int32 Count) override;
	virtual void GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const override;
	virtual void SetBacktrackingEnabled(const bool bBacktracking) override;
	virtual bool IsBacktrackingEnabled() const override;
	virtual float GetHeuristicScale() const override;
	virtual bool IsEqual(const INavigationQueryFilterInterface* Other) const override;
	virtual void SetIncludeFlags(uint16 Flags) override;
	virtual uint16 GetIncludeFlags() const override;
	virtual void SetExcludeFlags(uint16 Flags) override;
	virtual uint16 GetExcludeFlags() const override;


	virtual INavigationQueryFilterInterface* CreateCopy() const override;

	const dtQueryFilter* GetAsDetourQueryFilter() const { return this; }

	void SetIsVirtual(bool bIsVirtual);
};

/*
* Filter for Team One
*/
class SQUADLEADER_API dtQueryFilter_SL_TeamOne : public dtQueryFilter {
public:
	dtQueryFilter_SL_TeamOne(bool inIsVirtual = true) : dtQueryFilter(inIsVirtual) {}

	virtual ~dtQueryFilter_SL_TeamOne() {}

protected:
	virtual float getVirtualCost(const float* pa, const float* pb,
		const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
		const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
		const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const override;
private:
	float GetCostInfluenceMap(const FVector2D StartPosition, const FVector2D EndPosition) const;

public:
	AInfluenceMapGrid* InfluenceMap;
	void SetInfluenceMap(AInfluenceMapGrid* influenceMap);
};
class SQUADLEADER_API FRecatsQueryFilter_SL_TeamOne : public INavigationQueryFilterInterface, public dtQueryFilter_SL_TeamOne {
public:
	FRecatsQueryFilter_SL_TeamOne(bool bIsVirtual = true);
	~FRecatsQueryFilter_SL_TeamOne() {}

	virtual void Reset() override;

	virtual void SetAreaCost(uint8 AreaType, float Cost) override;
	virtual void SetFixedAreaEnteringCost(uint8 AreaType, float Cost) override;
	virtual void SetExcludedArea(uint8 AreaType) override;
	virtual void SetAllAreaCosts(const float* CostArray, const int32 Count) override;
	virtual void GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const override;
	virtual void SetBacktrackingEnabled(const bool bBacktracking) override;
	virtual bool IsBacktrackingEnabled() const override;
	virtual float GetHeuristicScale() const override;
	virtual bool IsEqual(const INavigationQueryFilterInterface* Other) const override;
	virtual void SetIncludeFlags(uint16 Flags) override;
	virtual uint16 GetIncludeFlags() const override;
	virtual void SetExcludeFlags(uint16 Flags) override;
	virtual uint16 GetExcludeFlags() const override;


	virtual INavigationQueryFilterInterface* CreateCopy() const override;

	const dtQueryFilter* GetAsDetourQueryFilter() const { return this; }

	void SetIsVirtual(bool bIsVirtual);
};

/**
 * AMyRecastNavMesh 
 */
UCLASS()
class SQUADLEADER_API AMyRecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()
	
public:
	AMyRecastNavMesh(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float deltaTime) override;
	virtual void BeginPlay() override;

private:
	FRecatsQueryFilter_SL* DefaultNavFilter{};
	FRecatsQueryFilter_SL_TeamOne* TeamOneNavFilter{};
	void SetupCustomNavFilter();

private:
	AInfluenceMapGrid* InfluenceMap;
	void InitInfluenceMap();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UNavigationQueryFilter> FilterTeam1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UNavigationQueryFilter> FilterTeam2;
};