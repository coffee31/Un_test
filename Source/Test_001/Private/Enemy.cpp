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
	
	// ���忡 �ִ� �÷��̾� ĳ���͸� ã�´�.
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
		//DieAction();
		break;
	case EEnemyState::RETURN:
		//MoveActiodn();
		ReRotate();
		break;
	}

	//���� ü���� ����;
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
		//�˹�
		// �ǰ� �ִϸ��̼� ��Ÿ�� �÷���
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
	//�ڽ��� ���濡 �÷��̾� üũ
	// ������·� ��ȯ
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
		//SetActorLocation(GetActorLocation() + dir * moveSpeed * GetWorld()->DeltaTimeSeconds, true);
		if (aicon != nullptr && player->GetCurrentHP() > 0)
		{
			UWorld* currentworld = GetWorld();

			aicon->MoveToActor(target, targetDis);

			//debug ������ �̿��ؼ� �̵� ��Ʈ�� �ð�ȭ��
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

	if (player != nullptr)
	{
		if (FVector::Distance(player->GetActorLocation(), GetActorLocation()) >= AttackDistance)
		{
			enemyState = EEnemyState::MOVE;
			return;
		}
		// 2. ���� ��ȯ�� �̿��� ���
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
	// 3�ʵ� ����
	FTimerHandle dieTimer;
	//GetWorld()->GetTimerManager(); ���Ͱ� �����ؾ���

	GetWorldTimerManager().SetTimer(dieTimer, this, &AEnemy::DestroyProcess, 3.0f, false);
	
	//ĸ�� �ݸ��� off
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
		//�׺���̼� �̵��� �����.
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
