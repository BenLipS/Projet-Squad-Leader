// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridBase.generated.h"

/**
 * 
 */
UCLASS()
class SQUADLEADER_API UGridBase : public UObject
{
	GENERATED_BODY()
public:
	UGridBase();

	UGridBase(bool _valid, float _value, FVector _location);

	UFUNCTION()
		bool IsValid() const noexcept { return m_isValid; }

	UFUNCTION()
		void SetValue(float _value) noexcept { m_value = _value; }

	UFUNCTION()
		float GetValue() const noexcept { return m_value; }

	UFUNCTION()
		void SetIsValid(bool _valid) noexcept { m_isValid = _valid; }

	UFUNCTION()
		void SetLocation(FVector _location) noexcept { m_location = _location; }

	UFUNCTION()
		void DrawBase(UWorld* _world);
protected:

	UPROPERTY()
		bool m_isValid;

	UPROPERTY()
		float m_value;

	UPROPERTY()
		FVector m_location;

};
