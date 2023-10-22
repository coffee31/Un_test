// Fill out your copyright notice in the Description page of Project Settings.


#include "DipGameModeBase.h"
#include "MainUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"


void ADipGameModeBase::BeginPlay()
{

	Super::BeginPlay();

	
	if (mainWidget != nullptr)
	{
		//위젯 블루프린트를 생성한다.
		mainwidget_inst = CreateWidget<UMainUserWidget>(GetWorld(), mainWidget);

		if (mainwidget_inst != nullptr)
		{
			//생성자에 위젯 인스턴스를 뷰 포트에 표시
			mainwidget_inst->AddToViewport(1);
		}
	}

	pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	
}



//점수 추가 함수
void ADipGameModeBase::AddScore(int32 point)
{
	currentScore += point;

	//FString scoretext = FString::Printf(TEXT("Current Score : %d"), currentScore);
	FString scoretext = FString::FormatAsNumber(currentScore);

	if (mainwidget_inst != nullptr)
	{
		mainwidget_inst->text_score->SetText(FText::FromString(scoretext));
		mainwidget_inst->PlayAnimation(mainwidget_inst->pointupAnim);
		//mainwidget_inst->PlayAnimationForward(mainwidget_inst->pointupAnim);
	}

	
}

void ADipGameModeBase::LoadGameOverUI()
{
	if (GameOverWidget != nullptr)
	{
		gameoverwidget_inst = CreateWidget<UUserWidget>(GetWorld(), GameOverWidget);

		if (gameoverwidget_inst != nullptr)
		{
			gameoverwidget_inst->AddToViewport(0);

			pc->SetShowMouseCursor(true);

			//마우스 영역을 ui 영역으로 고정
			pc->SetInputMode(FInputModeUIOnly());

			//크로스헤어 ui를 안보이게 처리
			mainwidget_inst->img_CrossHair->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

