// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "My_char.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	player = Cast<AMy_char>(TryGetPawnOwner());

	if (player != nullptr) 
	{
		movecomp = player->GetCharacterMovement();
	}




	

}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (player != nullptr)
	{
		vertical = player->moveDir.X * movecomp->MaxWalkSpeed;
		horizontal = player->moveDir.Y* movecomp->MaxWalkSpeed;
		
		IsJump = movecomp->IsFalling();
		crouch = movecomp->IsCrouching();


	}
}
