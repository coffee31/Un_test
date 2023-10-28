// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=MySetting)
	class AEnemy* enemyActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=MySetting)
	EEnemyState OwnerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySetting)
	class AActor* ikactor; 


	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	//AnimNotify_ ²À ºÙ¿©¾ßµÊ
	/*
	UFUNCTION()
	void AnimNotify_AttackEnd();


	UFUNCTION()
	void AnimNotify_AttackDamage();
	*/

};
