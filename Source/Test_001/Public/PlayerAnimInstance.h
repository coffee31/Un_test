// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEST_001_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	float horizontal;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	float vertical;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	bool IsJump;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	bool crouch;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	class AMy_char* player;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category = Mysetting)
	class UCharacterMovementComponent* movecomp;



	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

};
