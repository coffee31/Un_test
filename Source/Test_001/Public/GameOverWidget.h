// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()



protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(BindWidget),Category = MySetting)
	class UButton* btn_restart;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(BindWidget),Category = MySetting)
	class UButton* btn_Quit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	const TSoftObjectPtr<UWorld> restartLevel;

private:
	UFUNCTION()
	void Restart();

	UFUNCTION()
	void Quit();

};
