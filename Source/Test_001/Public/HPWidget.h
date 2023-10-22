// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UHPWidget : public UUserWidget
{
	GENERATED_BODY()
	



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category=MySetting)
	class UTextBlock* text_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category=MySetting)
	class UProgressBar* pb_health;

	void SetOwnerName(FString name);

	void SetHP(int32 curHp, int32 maxHp);
};
