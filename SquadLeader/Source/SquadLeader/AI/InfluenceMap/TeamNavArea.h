// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "TeamNavArea.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UTeamNavArea : public UNavArea
{
	GENERATED_BODY()

public:
	UTeamNavArea();

	void InitTeamNavArea(float _cost) noexcept;
};
