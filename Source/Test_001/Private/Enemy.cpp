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
	StartLotation = GetActorLocation();
	StartRotation = GetActorRotation();

	if (player != nullptr)
	{
		target = player;
	}

	//ü��
	CurrentHP = maxHP;
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
	case EEnemyState::RETURN:
		//MoveActiodn();
		ReRotate();
		break;
	}

}

void AEnemy::OnDamager(int32 Damage)
{
	CurrentHP = FMath::Max(0, CurrentHP - Damage);
	KnockBackLocation = GetActorLocation() + GetActorForwardVector() * -1 * KnockBackRange;
	enemyState = EEnemyState::HIT;
}

void AEnemy::IdleAction()
{
	//�ڽ��� ���濡 �÷��̾� üũ
	// ������·� ��ȯ

	if (player != nullptr)
	{
		FVector dirvec = player->GetActorLocation() - GetActorLocation();
		

		float reslut = FVector::DotProduct(GetActorForwardVector(), dirvec.GetSafeNormal());

		if (reslut >= 0 && dirvec.Length() < 1000.0f && player->GetCurrentHP() > 0)
		{
			target = player;
			enemyState = EEnemyState::MOVE;
		}
	}

}

void AEnemy::MoveActiodn()
{
	//GetWorldTimerManager().ClearAllTimersForObject(this); ��� Ÿ�̸Ӹ� �����Ų��.
	//���� ������ FTimerHandle Timerhandle; �� ���  GetWorldTimerManager().ClearTimer(Timerhandle);�� ��� Ÿ�̸� ����
	

	if (target != nullptr)
	{
		FVector dir = target->GetActorLocation() - GetActorLocation();
		dir.Z = 0;
		dir.Normalize();

		// �̵��ϴ� �������� ȸ�� //dir.ToOrientationRotator();�� ����.
		FRotator rotation = dir.ToOrientationRotator();
		SetActorRotation(rotation);

		// �̵�
		SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds);
		


		// attack distance ���� ���ٽ� attack���� ����
		//FVector::Distatnce�� ������
		float distance = FVector::Dist(GetActorLocation(), target->GetActorLocation());

		if (distance <= AttackDistance)
		{
			enemyState = EEnemyState::ATTACKDELAY;
			// 1. Timer Manager�� �̿��� ���
			/* FTimerHandle Timerhandle;
				GetWorldTimerManager().SetTimer(Timerhandle, this, &AEnemy::�Լ�, ���ʸ��� ���� �� ������(2.0f), �ݺ��Ұ���?(ture�� ���� �ݺ� false�� �ѹ�),ó�� ���� ������)
			*/
		}
		if (FVector::Distance(StartLotation, GetActorLocation()) >= returnDistance && enemyState != EEnemyState::RETURN)
		{
			target = StartObject;
			enemyState = EEnemyState::RETURN;
		}
	}
	
}

void AEnemy::AttackAction()
{
	// ������ �ִ� ����� 2����
	// ���� Ÿ�̸� ���
	// �Ÿ� üũ
	if (FVector::Distance(player->GetActorLocation(), GetActorLocation()) >= AttackDistance)
	{
		enemyState = EEnemyState::MOVE;
		return;
	}

	 
	
	// 2. ���� ��ȯ�� �̿��� ���
	enemyState = EEnemyState::ATTACKDELAY;
	Attatcking();
}

void AEnemy::AttackDelayAction()
{
	attackDelay += GetWorld()->GetDeltaSeconds();

	if (attackDelay >= 2.0f)
	{
		enemyState = EEnemyState::ATTACK;
		attackDelay = 0;
	}

}

void AEnemy::HitAction()
{
	FVector newLoc = FMath::Lerp(GetActorLocation(), KnockBackLocation, 0.5f);

	if (FVector::Distance(newLoc, GetActorLocation()) < 5)
	{
		//newLoc = GetActorLocation();
		enemyState = EEnemyState::MOVE;
	}
	else 
	{
		SetActorLocation(newLoc);
	}

	
}

void AEnemy::DieAction()
{

}

void AEnemy::ReRotate()
{
	FVector dir = StartLotation - GetActorLocation();
	dir.Z = 0;
	SetActorRotation(dir.ToOrientationRotator());
	SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * moveSpeed * GetWorld()->GetDeltaSeconds());

	if (dir.Length() < 10)
	{
		SetActorLocation(StartLotation);
		SetActorRotation(StartRotation);
		enemyState = EEnemyState::IDLE;
	}
	
	


}

void AEnemy::Attatcking()
{
	player->DamagePlayer(AttackPower);
	UE_LOG(LogTemp, Warning, TEXT("Player C_HP : %d"), player->GetCurrentHP());

	if (player->GetCurrentHP() <= 0)
	{
		enemyState = EEnemyState::RETURN;
	}
}

