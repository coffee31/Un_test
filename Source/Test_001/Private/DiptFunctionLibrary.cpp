// Fill out your copyright notice in the Description page of Project Settings.


#include "DiptFunctionLibrary.h"

FRotator UDiptFunctionLibrary::CalculateRotationFromDirection(const AActor* base, const AActor* target)
{
	FVector dir = target->GetActorLocation() - base->GetActorLocation();
	dir.Normalize();

	FRotator calRot = dir.ToOrientationRotator();
	return calRot;
}
