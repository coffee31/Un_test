// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	//���� �� �ִϸ��̼� �������Ʈ�� �����ϰ� �ִ� ���ʹ� �ν���Ʈ�� ����
	enemyActor = Cast<AEnemy>(TryGetPawnOwner());



}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//���ʹ� ���°��� �޾Ƽ� enum ������ ����
	if (enemyActor != nullptr)
	{
		OwnerState = enemyActor->enemyState;
		
	}

}

/* NotifyŰ�� ã�Ƽ� �ڵ忡�� �ϴ¹�� [AttackEnd�� �Լ��� �ٲ�� �۵���]
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

