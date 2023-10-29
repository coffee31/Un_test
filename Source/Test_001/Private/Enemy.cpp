// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "My_char.h"
#include "GameFramework/pawn.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "DipGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "HPWidget.h"
#include "AIController.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"

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
	

	pawnmovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Comp"));
	ArrowComp->SetupAttachment(RootComponent);
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Comp"));
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	WidgetComp->SetDrawSize(FVector2D(300, 200));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 월드에 있는 플레이어 캐릭터를 찾는다.
	//AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMy_char::StaticClass());

	TArray<AActor*> charters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), charters);

	if (charters.Num() > 0)
	{
		for (AActor* playerActor : charters)
		{
			player = Cast<AMy_char>(playerActor);
			if (player != nullptr)
			{
				target = player;
				break;
			}
			else
			{
				bp_player = Cast<ACharacter>(playerActor);
				if (bp_player != nullptr)
				{
					target = bp_player;
					break;
				}
			}
		}
	}

	hpwidget = Cast<UHPWidget>(WidgetComp->GetWidget());
	

	if (hpwidget != nullptr)
	{
		hpwidget->SetOwnerName(GetActorNameOrLabel());
	}

	StartLotation = GetActorLocation();
	StartRotation = GetActorRotation();
	

	//SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds, true);
	aicon = GetController<AAIController>();

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
		//DieAction();
		break;
	case EEnemyState::RETURN:
		//MoveActiodn();
		ReRotate();
		break;
	}

	//현재 체력을 갱신;
	if (hpwidget != nullptr)
	{
		hpwidget->SetHP(CurrentHP, maxHP);
	}

	if (enemyState == EEnemyState::RETURN)
	{
		CurrentHP = maxHP;

	}
}

void AEnemy::OnDamager(int32 Damage)
{
	CurrentHP = FMath::Max(0, CurrentHP - Damage);

	if (CurrentHP <= 0)
	{
		enemyState = EEnemyState::DIE;
		DieAction();
		ADipGameModeBase* GM = Cast<ADipGameModeBase>(GetWorld()->GetAuthGameMode());

		if (GM != NULL)
		{
			GM->AddScore(1);
		}
		
	}
	else
	{
		//넉백
		// 피격 애니메이션 몽타주 플레이
		UAnimInstance* enemyAnimInstance = SkelMesh->GetAnimInstance();
		if (enemyAnimInstance != nullptr)
		{
			if(enemyState != EEnemyState::RETURN)
				enemyAnimInstance->Montage_Play(HitMontage);
		}
		
		if (enemyState != EEnemyState::RETURN)
		{
			FVector dir = target->GetActorLocation() - GetActorLocation();
			dir.Z = 0;
			dir.Normalize();

			FRotator rotation = dir.ToOrientationRotator();
			SetActorRotation(rotation);
		}

		KnockBackLocation = GetActorLocation() + GetActorForwardVector() * -1 * KnockBackRange;
		if (enemyState != EEnemyState::RETURN)
			enemyState = EEnemyState::HIT;
		else
			enemyState = EEnemyState::RETURN;
	}
	


}

void AEnemy::IdleAction()
{
	//자신의 전방에 플레이어 체크
	// 무브상태로 전환
	target = player;

	if (player != nullptr)
	{
		FVector dirvec = player->GetActorLocation() - GetActorLocation();

		float reslut = FVector::DotProduct(GetActorForwardVector(), dirvec.GetSafeNormal());
		//
		if (reslut >= 0 && dirvec.Length() < 1000.0f && player->GetCurrentHP() > 0)
		{
			target = player;
			enemyState = EEnemyState::MOVE;
		}

	}
	else
	{
		if (bp_player != nullptr)
		{
			FVector dirvec = bp_player->GetActorLocation() - GetActorLocation();

			float reslut = FVector::DotProduct(GetActorForwardVector(), dirvec.GetSafeNormal());
			//
			if (reslut >= 0 && dirvec.Length() < 1000.0f)
			{
				target = bp_player;
				enemyState = EEnemyState::MOVE;
			}
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
		//SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds, true);
		if (aicon != nullptr && player->GetCurrentHP() > 0)
		{
			UWorld* currentworld = GetWorld();

			aicon->MoveToActor(target, targetDis);

			//debug 라인을 이용해서 이동 루트를 시각화함
			UNavigationSystemV1* nv1 = UNavigationSystemV1::GetCurrent(currentworld);
			if (nv1 != nullptr)
			{
				
				UNavigationPath* navPath = nv1->FindPathToLocationSynchronously(currentworld, GetActorLocation(), target->GetActorLocation());

				if (navPath != nullptr)
				{
					TArray<FVector> points = navPath->PathPoints;

					for (int32 i = 0; i < points.Num() - 1; i++)
					{
						DrawDebugLine(GetWorld(), points[i], points[i + 1], FColor::Red, false, 0, 0, 2.0f);

						DrawDebugSphere(currentworld, points[i], 5, 12, FColor::Blue, false, 0, 0, 2.0f);
					}
				}
			}
		}



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

	if (player != nullptr)
	{
		if (FVector::Distance(player->GetActorLocation(), GetActorLocation()) >= AttackDistance)
		{
			enemyState = EEnemyState::MOVE;
			return;
		}
		// 2. 상태 전환을 이용한 방법
		//enemyState = EEnemyState::ATTACKDELAY;
		//Attatcking();
	}

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
	FVector newLoc = FMath::Lerp(GetActorLocation(), KnockBackLocation, 0.3f);

	if (FVector::Distance(newLoc, KnockBackLocation) <= 20)
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
	UAnimInstance* enemyAnimInstance = SkelMesh->GetAnimInstance();
	if (enemyAnimInstance != nullptr)
	{
		enemyAnimInstance->Montage_Play(DeathMontage);
	}
	// 3초뒤 제거
	FTimerHandle dieTimer;
	//GetWorld()->GetTimerManager(); 액터가 존재해야함

	GetWorldTimerManager().SetTimer(dieTimer, this, &AEnemy::DestroyProcess, 3.0f, false);
	
	//캡슐 콜리전 off
	Capsulecomp->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}

void AEnemy::ReRotate()
{
	FVector dir = StartLotation - GetActorLocation();
	dir.Z = 0;
	SetActorRotation(dir.ToOrientationRotator());
	//SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * moveSpeed * GetWorld()->GetDeltaSeconds());
	if (aicon != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("succeed!"));
		FAIMoveRequest req = FAIMoveRequest(StartLotation);

		req.SetAcceptanceRadius(1.0f);
		req.SetUsePathfinding(true);
		req.SetProjectGoalLocation(false);
		req.SetReachTestIncludesGoalRadius(true);
		req.SetNavigationFilter(aicon->GetDefaultNavigationFilterClass());
		req.SetCanStrafe(true);
		req.SetAllowPartialPath(true);

		aicon->MoveTo(req);
		
		//aicon->MoveToLocation(StartLotation);
	}



	if (dir.Length() < 100)
	{
		SetActorLocation(StartLotation);
		SetActorRotation(StartRotation);
		//네비게이션 이동을 멈춘다.
		aicon->StopMovement();
		enemyState = EEnemyState::IDLE;
	}

}

void AEnemy::Attatcking()
{
	if (player != nullptr)
	{
		player->DamagePlayer(AttackPower);
		UE_LOG(LogTemp, Warning, TEXT("Player C_HP : %d"), player->GetCurrentHP());

		if (player->GetCurrentHP() <= 0)
		{
			enemyState = EEnemyState::RETURN;
		}
	}
}

void AEnemy::DestroyProcess()
{
	Destroy();
}
