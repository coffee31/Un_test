// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mysetting, meta=(BindWidget))
	class UTextBlock* text_score;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mysetting)
	int32 currentScore = 0;

};
