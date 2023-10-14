// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//헤더에서 include 시 무조건 맨아래에 클래스명.generated.h가 와야함

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "My_char.generated.h"


UCLASS()
class TEST_001_API AMy_char : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMy_char();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//자주 사용하는 것들 EditAnywhere / Visible /editdefalut도 있음
	//언리얼에선 int32로 사용하는것이 보편적

	UPROPERTY(EditAnywhere, Category = MySetting) // 매크로 함수에 할당 바로 아래 한줄만 적용됨
	class UCameraComponent* CameraComp; // 포인터 변수 선언

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditAnywhere)
	int32 test1 = 20;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputMappingContext* MyIMC_File;


	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_jump;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_move;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_rotate;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_Crouch;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_Dash;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_Thorw;


	UPROPERTY(EditAnywhere, Category = MySetting)
	class UInputAction* ia_Fire;

	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class AActor> bullet_bp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UParticleSystem* hit_fx;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USoundBase* fire_sound;

	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class AGranadeActor> Granade_bp;

	//UFUNCTION(BlueprintCallable, DisplayName = "MY LT") // 실행핀 있음
	UFUNCTION(BlueprintPure) // 실행핀 없음
	bool MyLineTraceMultiByChannel(TArray<struct FHitResult>& _hitinfos, const FVector _start, const FVector _end, enum ECollisionChannel _Ecc);

	//call funtion에 만든 함수가 생김





private:
	//Transform 관련은 CoreMinimal.헤더에서 이미 class인지 struct인지 전방선언 되어있어 생략가능하지만 이 외에는 알 수 없기에 전방선언 해줘야함.
	FVector moveDir;
	FRotator rotateAxis;
	class APlayerController* pc;

	//백터와 같이 특정한 애들 제외하곤 class 또는 Struct 및 기타 등등을 전방선언 해야함.

	void OnMoveInput(const struct FInputActionValue& value);
	void OnRotateInput(const struct FInputActionValue& value);
	void DashON();
	void DashOFF();
	void CrouchON();
	void CrouchOFF();
	void ONThrow();

	void OnFireInput(const struct FInputActionValue& value);

	

	// 모듈을 열려면 솔류션에서 Build.cs에서 해야됨


	/*
	1. InputAction으로 키 파일 할당
	2. EnhancedInputComponent로 BindAction(키,함수)
	3. 이벤트 함수의 구현부 작성
	*/
};
