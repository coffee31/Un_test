// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

// 에너미 상태
UENUM(BlueprintType)
enum class EEnemyState
{
	IDLE UMETA(DisplayName = "Idle State"),
	MOVE,
	ATTACK,
	ATTACKDELAY,
	HIT,
	DIE,
};








UCLASS()
class TEST_001_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySetting)
	EEnemyState enemyState = EEnemyState::IDLE;


private:
	UPROPERTY() // 사용자 정의 클래스를 언리얼에서 빠르게 읽어 들이기 위한 프로퍼티
	class AMy_char* player;
	

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USkeletalMeshComponent* SkelMesh;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UCapsuleComponent* Capsulecomp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UArrowComponent* ArrowComp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	float moveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = MySetting)
	float AttackDistance = 150.0f;

	void IdleAction();
	void MoveActiodn();
	void AttackAction();
	void AttackDelayAction();
	void HitAction();
	void DieAction();

};
