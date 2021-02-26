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

	/*
	* Check of the GridBase is valid or not
	*/
	UFUNCTION(BlueprintCallable)
		bool IsValid() const noexcept { return m_isValid; }

	UFUNCTION(BlueprintCallable)
		void SetValue(float _value) noexcept { m_value = _value; }

	UFUNCTION(BlueprintCallable)
		float GetValue() const noexcept { return m_value; }

	UFUNCTION(BlueprintCallable)
		void SetIsValid(bool _valid) noexcept { m_isValid = _valid; }

	UFUNCTION(BlueprintCallable)
		FVector GetLocation() const noexcept { return m_location; }
	UFUNCTION(BlueprintCallable)
		void SetLocation(FVector _location) noexcept { m_location = _location; }

	/*
	* Draw a DebugSolidBox
	* only use this if you want to debbug
	*/
	UFUNCTION(BlueprintCallable)
		void DrawBase(UWorld* _world);

protected:
	/*
	* Boolean to know if the GridBase is Valid or not
	*/
	UPROPERTY()
		bool m_isValid;

	/*
	* The data of the GridBase
	* the value is between [-1; 1]
	*/
	UPROPERTY()
		float m_value;

	/*
	* Only use for debbuging the code
	*/
	UPROPERTY()
		FVector m_location;

};
