// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DipGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API ADipGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySetting)
	int32 currentScore = 0;

	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class UMainUserWidget> mainWidget;

	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class UUserWidget> GameOverWidget;

	void AddScore(int32 point);
	void LoadGameOverUI();




protected:
	virtual void BeginPlay() override;


private:
	class UMainUserWidget* mainwidget_inst;
	class UUserWidget* gameoverwidget_inst;
	class APlayerController* pc;

};
