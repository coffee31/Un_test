// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//������� include �� ������ �ǾƷ��� Ŭ������.generated.h�� �;���

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

	//���� ����ϴ� �͵� EditAnywhere / Visible /editdefalut�� ����
	//�𸮾󿡼� int32�� ����ϴ°��� ������

	UPROPERTY(EditAnywhere, Category = MySetting) // ��ũ�� �Լ��� �Ҵ� �ٷ� �Ʒ� ���ٸ� �����
	class UCameraComponent* CameraComp; // ������ ���� ����

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

	//UFUNCTION(BlueprintCallable, DisplayName = "MY LT") // ������ ����
	UFUNCTION(BlueprintPure) // ������ ����
	bool MyLineTraceMultiByChannel(TArray<struct FHitResult>& _hitinfos, const FVector _start, const FVector _end, enum ECollisionChannel _Ecc);

	//call funtion�� ���� �Լ��� ����





private:
	//Transform ������ CoreMinimal.������� �̹� class���� struct���� ���漱�� �Ǿ��־� �������������� �� �ܿ��� �� �� ���⿡ ���漱�� �������.
	FVector moveDir;
	FRotator rotateAxis;
	class APlayerController* pc;

	//���Ϳ� ���� Ư���� �ֵ� �����ϰ� class �Ǵ� Struct �� ��Ÿ ����� ���漱�� �ؾ���.

	void OnMoveInput(const struct FInputActionValue& value);
	void OnRotateInput(const struct FInputActionValue& value);
	void DashON();
	void DashOFF();
	void CrouchON();
	void CrouchOFF();
	void ONThrow();

	void OnFireInput(const struct FInputActionValue& value);

	

	// ����� ������ �ַ��ǿ��� Build.cs���� �ؾߵ�


	/*
	1. InputAction���� Ű ���� �Ҵ�
	2. EnhancedInputComponent�� BindAction(Ű,�Լ�)
	3. �̺�Ʈ �Լ��� ������ �ۼ�
	*/
};
