// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DiptFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UDiptFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	//base ���Ͱ� target ���͸� �ٶ󺸵��� ���� ȸ�� ���� ��ȯ�ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category =MyFunction)
	static FRotator CalculateRotationFromDirection(const AActor* base, const AActor* target);


};
