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
		//���� �������Ʈ�� �����Ѵ�.
		mainwidget_inst = CreateWidget<UMainUserWidget>(GetWorld(), mainWidget);

		if (mainwidget_inst != nullptr)
		{
			//�����ڿ� ���� �ν��Ͻ��� �� ��Ʈ�� ǥ��
			mainwidget_inst->AddToViewport(1);
		}
	}

	pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	
}



//���� �߰� �Լ�
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

			//���콺 ������ ui �������� ����
			pc->SetInputMode(FInputModeUIOnly());

			//ũ�ν���� ui�� �Ⱥ��̰� ó��
			mainwidget_inst->img_CrossHair->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

