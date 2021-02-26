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
	DrawDebugSolidBox(_world, m_location, FVector(95.f, 95.f, 10.f), FColor::Black);
}
