// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBase.h"
#include "DrawDebugHelpers.h"

UGridBase::UGridBase() {
	m_value = 0.f;
	m_isValid = false;
	m_location = FVector(0.f, 0.f, 0.f);
}

UGridBase::UGridBase(bool _valid, float _value, FVector _location) : m_isValid{ _valid }, m_value{ _value }, m_location{ _location }{}

void UGridBase::DrawBase(UWorld* _world) {
	//DrawDebugBox(_world, m_location, FVector(5.f, 5.f, 30.f), FQuat(), FColor::Red);
	//DrawDebugPoint(_world, m_location, 20.f, FColor::Red);
	DrawDebugSolidBox(_world, m_location, FVector(95.f, 95.f, 10.f), FColor::Black);
	
}