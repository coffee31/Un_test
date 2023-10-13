// Fill out your copyright notice in the Description page of Project Settings.


#include "My_char.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <BulletActor.h>

// Sets default values
AMy_char::AMy_char()
{
	/*
	������
	�� #Include���� �� ���� �������� Generate �ѹ� �ؾߵ�

	�ڵ� ����
	1. CreateDefaultSubobject -> ������Ʈ ����
	2. SetupAttachment -> ��� ������Ʈ�� �ش� ������Ʈ�� �ڽ����� ���� ������ (ex_ RootComponent<�ֻ�� ������Ʈ>)
	3. TargetArmLength -> SpringArm�� �Ÿ� ����
	4. ���̷�Ż �޽� �κ��� ������ üũ

	*/
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm")); // Spring Arm ������Ʈ ����
	SpringArmComp->SetupAttachment(RootComponent); //���� �ֻ�� ������Ʈ�� �ڽ����� ����
	SpringArmComp->TargetArmLength = 500.0f;

	// ī�޶� ������Ʈ�� ������ �� ������Ʈ�� �ڽ� ������Ʈ�� ���� �� �����Ѵ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(SpringArmComp); // SpringArm �ȿ� ī�޶� �ڽ����� ����

	//�� ������ �����ֱ� ���� ���̷�Ż �޽� ������Ʈ�� �ٵ� �޽��� �ڽ� ������Ʈ�� ���� �� ����
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Comp"));
	GunMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket"));

	// ���̷�Ż �޽� ������ �����ؼ� SkeletalMesh Component�� �ֱ�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

	if (playerMesh.Succeeded())
	{
		USkeletalMeshComponent* myMesh = GetMesh();
		myMesh->SetSkeletalMesh(playerMesh.Object);
		myMesh->SetRelativeLocation(FVector(0, 0, -90.0f));
		myMesh->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}

	//�⺻ �̵� �ӵ��� 2400cm/s�� ������.
	//�Ҿ����� Ŭ������� ���� -> #include �ؾߵ�;

	//vs assist���� alt G�� ���� [f12�� ����� ���] �� ���� �����
	GetCharacterMovement()->MaxWalkSpeed = 600;
	JumpMaxCount = 3;

	// Crouch �۵� �� �� �ֵ��� ����
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}


void AMy_char::BeginPlay()
{

	/*
	������ ����
	

	�ڵ� ����
	1. APlayerController�� ���� ĳ���ͷ� ���Ǵ� ��. ���� �� ��ɵ��� ����ü
	2. GetController<APlayerController>(); ���� �÷��̾��� ��Ʈ�ѷ��� ������(Get)
	3. EnhancedInputLocalPlayer�� ���� ���� üũ
	4. Input Mapping Context�� ���� ���� üũ

	*/
	Super::BeginPlay();
	
	// ���� �÷��̾� ��Ʈ�ѷ��� ������. ctr + cast + set ���� ����
	pc = GetController<APlayerController>();
	
	if (pc != nullptr) 
	{
		// Enhanced Input Local Player Subsystem ��������
		UEnhancedInputLocalPlayerSubsystem* enhancedInputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());


		//Input Mapping Context ������ ����
		if (enhancedInputSubsys != nullptr)
		{
			enhancedInputSubsys->AddMappingContext(MyIMC_File, 0);
		}

	}
	

}

// Called every frame
void AMy_char::Tick(float DeltaTime)
{
	/*
	������ 
	�� �ڵ�󿡼� FRotator�� pitch yaw roll ����
	
	Pitch = ���� || Yaw = �¿� || Roll = �յ�


	�ڵ� ����
	1. ����ȭ ����� moveDir.Normalize(); AddMovementInput(moveDir)�� / AddMovementInput(moveDir.GetSafeNormal()); 2���� ����� ����.
	2. Normalize�� AddMovementInput�� �ȵ���
	3. Normalize�� ���� ��ȯ��Ű�� ��� / GetSafeNormal�� ��ȯ�� �ѱ�� ���
	4. AddMovementInput -> ĳ���� �̵� / p = p0+vt
	5. UE_LOG(ī�װ�, ���� or Log,Warning,Error,Fatal ǥ��, Fatal�� �����͸� ũ����, TEXT)
	6. AddControllerYawInput -> ĳ���� �¿� ȸ�� �� ��Ʈ�ѷ� ��� ȸ��

	*/

	Super::Tick(DeltaTime);

	moveDir.Normalize();
		

	//�ٶ󺸴� �������� �̵�
	//moveDir = pc->GetControlRotation() * moveDir.X + pc->GetControlRotation() * moveDir.Y;
	FVector forwardvector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightvector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);
	moveDir = (forwardvector * moveDir.X + rightvector * moveDir.Y).GetSafeNormal(); //2 ���͸� ����ϰ� safenormal�� ��ȯ�� ���� moveDir�� �ִ´�.

	//�̵��ӵ��� ����Ʈ���� ĳ���� MoveMent ������Ʈ���� Walking�� �ִ� Max speed ����
	AddMovementInput(moveDir);

	

	//Log ���
	//UE_LOG(LogTemp, Warning, TEXT("Yaw: %f, Pitch: %f"),rotateAxis.Yaw, rotateAxis.Pitch);

	//ĳ���� �¿� ȸ�� - �÷��̾� ��Ʈ�ѷ� ȸ�� ���
	AddControllerYawInput(rotateAxis.Yaw);
	
	//ĳ���� ���� ȸ�� - ������ �� ȸ�� ���
	FRotator curCamRot = SpringArmComp->GetComponentRotation(); // ���� ī�޶� �����̼��� ������ ���� �����̼� ������Ʈ�� ������
	
	float modifyPitch = curCamRot.Pitch + rotateAxis.Pitch;
	//curCamRot + FRotator(rotateAxis.Pitch, 0, 0); -> ������ �����̼��� ��������̼� + ���콺 Pitch ����
	//�Ʒ��� ���������� ������ �Ѿ�� �ʰ� -60~60���� ����
	modifyPitch = FMath::Clamp(modifyPitch, -60, 60);
	FRotator ModifiedRot = FRotator(modifyPitch, curCamRot.Yaw, curCamRot.Roll);

	SpringArmComp->SetWorldRotation(ModifiedRot); //������ �����̼��� SetWroldRotation���� ��.

	

}

// Called to bind functionality to input
void AMy_char::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//���� �Է� �̺�Ʈ ������ EnhancdeInput ���·� ĳ���� ��.
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);


	// �Է� Ű�� ������ �Լ��� ����Bind �ϱ�
	if (enhancedInputComponent != nullptr)
	{
		//Jump
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping); // �̹� ��������ִ°ű⿡ �θ� Ŭ������ ACharacter

		//Move
		enhancedInputComponent->BindAction(ia_move, ETriggerEvent::Triggered, this, &AMy_char::OnMoveInput); // �츮�� ����ű⿡ Mychar
		enhancedInputComponent->BindAction(ia_move, ETriggerEvent::Completed, this, &AMy_char::OnMoveInput);

		//Rotate
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Triggered, this, &AMy_char::OnRotateInput);
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Completed, this, &AMy_char::OnRotateInput);

		//Dash
		//enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Started, this, &AMy_char::DashON);
		//enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Completed, this, &AMy_char::DashOFF);

		//Crouch
		enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Started, this, &AMy_char::DashON);
		enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Completed, this, &AMy_char::DashOFF);

		//
		enhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Started, this, &AMy_char::OnFireInput);
	}

}

void AMy_char::OnMoveInput(const FInputActionValue& value)
{
	// Ű �Է� �̺�Ʈ �߻� �ÿ� ���� value ���� Fvector2D���·� ��ȯ

	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}


void AMy_char::OnRotateInput(const FInputActionValue& value)
{
	//yaw ���� (�¿�) pitch�� (����)
	//Fvector2D�� �޾Ƽ� ��ȯ�ϱ� ������ �Ʒ�ó����.
	FVector2D rotateInput = value.Get<FVector2D>();

	rotateAxis.Pitch = rotateInput.Y;
	rotateAxis.Yaw = rotateInput.X;

	//�������Ʈ ���

}

void AMy_char::DashON()
{
	//GetCharacterMovement()->MaxWalkSpeed = 2400.0f;
	Crouch();
}
void AMy_char::DashOFF()
{
	//GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	UnCrouch();
}

void AMy_char::OnFireInput(const FInputActionValue& value)
{
	
	//1. �������Ʈ ���� ����
	/*
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	*/
	


	//2. �ڵ�� ����

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	/*
	if(spawnBullet != nullptr)
	{
		spawnBullet->StaticMeshComp->SetWorldScale3D(FVector(1f, 1f, 1f)); �̰͵� ������

	}
	*/
	



}

