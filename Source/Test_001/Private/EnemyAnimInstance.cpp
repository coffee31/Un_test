// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	//현재 이 애니메이션 블루프린트를 소유하고 있는 에너미 인스턴트를 저장
	enemyActor = Cast<AEnemy>(TryGetPawnOwner());



}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//에너미 상태값을 받아서 enum 변수에 저장
	if (enemyActor != nullptr)
	{
		OwnerState = enemyActor->enemyState;
		
	}

}

/* Notify키값 찾아서 코드에서 하는방법 [AttackEnd가 함수로 바꿔야 작동됨]
void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	if (enemyActor != nullptr)
	{
		enemyActor->AttackEnd();
	}
}

void UEnemyAnimInstance::AnimNotify_AttackDamage()
{
	if (enemyActor != nullptr)
	{
		enemyActor->Attatcking();
	}
}
*/

