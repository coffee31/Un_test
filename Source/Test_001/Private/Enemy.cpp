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
	
	// 월드에 있는 플레이어 캐릭터를 찾는다.
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMy_char::StaticClass());
	player = Cast<AMy_char>(playerActor);
	StartLotation = GetActorLocation();
	StartRotation = GetActorRotation();

	if (player != nullptr)
	{
		target = player;
	}

	//체력
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
	//자신의 전방에 플레이어 체크
	// 무브상태로 전환

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
	//GetWorldTimerManager().ClearAllTimersForObject(this); 모든 타이머를 종료시킨다.
	//전역 변수로 FTimerHandle Timerhandle; 인 경우  GetWorldTimerManager().ClearTimer(Timerhandle);로 대상 타이머 종료
	

	if (target != nullptr)
	{
		FVector dir = target->GetActorLocation() - GetActorLocation();
		dir.Z = 0;
		dir.Normalize();

		// 이동하는 방향으로 회전 //dir.ToOrientationRotator();도 있음.
		FRotator rotation = dir.ToOrientationRotator();
		SetActorRotation(rotation);

		// 이동
		SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds);
		


		// attack distance 까지 접근시 attack으로 변경
		//FVector::Distatnce도 가능함
		float distance = FVector::Dist(GetActorLocation(), target->GetActorLocation());

		if (distance <= AttackDistance)
		{
			enemyState = EEnemyState::ATTACKDELAY;
			// 1. Timer Manager를 이용한 방법
			/* FTimerHandle Timerhandle;
				GetWorldTimerManager().SetTimer(Timerhandle, this, &AEnemy::함수, 몇초마다 실행 할 것인지(2.0f), 반복할거임?(ture면 무한 반복 false면 한번),처음 시작 딜레이)
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
	// 딜레이 주는 방법은 2가지
	// 위의 타이머 방법
	// 거리 체크
	if (FVector::Distance(player->GetActorLocation(), GetActorLocation()) >= AttackDistance)
	{
		enemyState = EEnemyState::MOVE;
		return;
	}

	 
	
	// 2. 상태 전환을 이용한 방법
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

