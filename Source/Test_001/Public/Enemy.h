// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

// 에너미 상태
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE UMETA(DisplayName = "Idle State"),
	MOVE,
	ATTACK,
	ATTACKDELAY,
	RETURN,
	HIT,
	DIE,
};


UCLASS()
class TEST_001_API AEnemy : public APawn
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

	UPROPERTY(EditAnywhere, Category = MySetting)
	float KnockBackRange = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class AAIController* aicon;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	int32 maxHP = 500;
	
	UPROPERTY(EditAnywhere, Category = MySetting)
	class AActor* StartObject;

	UFUNCTION(BlueprintCallable)
	void OnDamager(int32 Damage);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentHP() { return CurrentHP; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)// 사용자 정의 클래스를 언리얼에서 빠르게 읽어 들이기 위한 프로퍼티
	class AMy_char* player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	int32 CurrentHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class AActor* boxActor;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySetting)
	class UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintCallable)
	void Attatcking();

private:

	class UHPWidget* hpwidget;
	FVector KnockBackLocation;



	UPROPERTY() // 사용자 정의 클래스를 언리얼에서 빠르게 읽어 들이기 위한 프로퍼티
	class ACharacter* bp_player;
	
	UPROPERTY(EditAnywhere, Category = MySetting)
	class AActor* target;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USkeletalMeshComponent* SkelMesh;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UCapsuleComponent* Capsulecomp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UArrowComponent* ArrowComp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UFloatingPawnMovement* pawnmovement;


	UPROPERTY(EditAnywhere, Category = MySetting)
	float moveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = MySetting)
	float targetDis = 50.0f;

	UPROPERTY(EditAnywhere, Category = MySetting)
	float AttackDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = MySetting, meta=(AllowPrivateAccess = true))
	float AttackPower = 20;

	float attackDelay = 0;

	UPROPERTY(EditAnywhere, Category = MySetting)
	float returnDistance = 3000.0f;

	FVector StartLotation;
	FRotator StartRotation;

	

	void IdleAction();
	void MoveActiodn();
	void AttackAction();
	void AttackDelayAction();
	void HitAction();
	void DieAction();
	void ReRotate();

	void DestroyProcess();


};
