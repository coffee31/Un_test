// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "My_char.h"
#include "GameFramework/pawn.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsulecomp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
	SetRootComponent(Capsulecomp);
	Capsulecomp->SetCapsuleHalfHeight(110.0f);

	SkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Static Mesh Comp"));
	SkelMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> enemyBody(TEXT("/Game/Animations/Enemy/Enemy_Mesh"));

	if (enemyBody.Succeeded())
	{
		SkelMesh->SetSkeletalMesh(enemyBody.Object);
	}
	

	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Comp"));
	ArrowComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// ���忡 �ִ� �÷��̾� ĳ���͸� ã�´�.
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMy_char::StaticClass());
	player = Cast<AMy_char>(playerActor);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (enemyState)
	{
	case EEnemyState::IDLE:
		IdleAction();
		break;
	case EEnemyState::MOVE:
		MoveActiodn();
		break;
	case EEnemyState::ATTACK:
		AttackAction();
		break;
	case EEnemyState::ATTACKDELAY:
		AttackDelayAction();
		break;
	case EEnemyState::HIT:
		HitAction();
		break;
	case EEnemyState::DIE:
		DieAction();
		break;
	}

}

void AEnemy::IdleAction()
{
	//�ڽ��� ���濡 �÷��̾� üũ
	// ������·� ��ȯ

	if (player != nullptr)
	{
		FVector dirvec = player->GetActorLocation() - GetActorLocation();

		float reslut = FVector::DotProduct(GetActorForwardVector(), dirvec.GetSafeNormal());

		if (reslut >= 0 && dirvec.Length() < 700.0f)
		{
			enemyState = EEnemyState::MOVE;
		}
	}

	
	


}

void AEnemy::MoveActiodn()
{


	FVector dir = player->GetActorLocation() - GetActorLocation();
	dir.Normalize();

	// �̵��ϴ� �������� ȸ��
	FRotator rotation = player->GetActorRotation() - GetActorRotation();
	SetActorRotation(rotation);

	// �̵�

	SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds);


	// attach distance ���� ���ٽ� attack���� ����

	
}

void AEnemy::AttackAction()
{
}

void AEnemy::AttackDelayAction()
{

}

void AEnemy::HitAction()
{

}

void AEnemy::DieAction()
{

}

