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
	주의점
	※ #Include했을 때 만약 오류나면 Generate 한번 해야됨

	코드 관련
	1. CreateDefaultSubobject -> 오브젝트 생성
	2. SetupAttachment -> 어느 컴포넌트에 해당 컴포넌트를 자식으로 넣을 것인지 (ex_ RootComponent<최상단 컴포넌트>)
	3. TargetArmLength -> SpringArm의 거리 변수
	4. 스켈레탈 메시 부분은 집가서 체크

	*/
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm")); // Spring Arm 컴포넌트 생성
	SpringArmComp->SetupAttachment(RootComponent); //가장 최상단 컴포넌트에 자식으로 넣음
	SpringArmComp->TargetArmLength = 500.0f;

	// 카메라 컴포넌트를 스프링 암 컴포넌트의 자식 컴포넌트로 생성 및 부착한다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(SpringArmComp); // SpringArm 안에 카메라를 자식으로 생성

	//총 외형을 보여주기 위한 스켈레탈 메시 컴포넌트를 바디 메시의 자식 컴포넌트로 생성 및 부착
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Comp"));
	GunMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket"));

	// 스켈레탈 메시 파일을 생성해서 SkeletalMesh Component에 넣기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

	if (playerMesh.Succeeded())
	{
		USkeletalMeshComponent* myMesh = GetMesh();
		myMesh->SetSkeletalMesh(playerMesh.Object);
		myMesh->SetRelativeLocation(FVector(0, 0, -90.0f));
		myMesh->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}

	//기본 이동 속도를 2400cm/s로 조절함.
	//불안정한 클래스라는 오류 -> #include 해야됨;

	//vs assist에는 alt G가 있음 [f12랑 비슷한 기능] 더 좋은 기능임
	GetCharacterMovement()->MaxWalkSpeed = 600;
	JumpMaxCount = 3;

	// Crouch 작동 할 수 있도록 설정
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}


void AMy_char::BeginPlay()
{

	/*
	주의점 없음
	

	코드 관련
	1. APlayerController는 메인 캐릭터로 사용되는 것. 조작 및 기능들의 집합체
	2. GetController<APlayerController>(); 현재 플레이어의 컨트롤러를 가져옴(Get)
	3. EnhancedInputLocalPlayer에 대한 내용 체크
	4. Input Mapping Context에 대한 내용 체크

	*/
	Super::BeginPlay();
	
	// 현재 플레이어 컨트롤러를 가져옴. ctr + cast + set 까지 다함
	pc = GetController<APlayerController>();
	
	if (pc != nullptr) 
	{
		// Enhanced Input Local Player Subsystem 가져오기
		UEnhancedInputLocalPlayerSubsystem* enhancedInputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());


		//Input Mapping Context 파일을 맵핑
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
	주의점 
	※ 코드상에선 FRotator는 pitch yaw roll 순서
	
	Pitch = 상하 || Yaw = 좌우 || Roll = 앞뒤


	코드 관련
	1. 정규화 방식은 moveDir.Normalize(); AddMovementInput(moveDir)와 / AddMovementInput(moveDir.GetSafeNormal()); 2가지 방식이 있음.
	2. Normalize는 AddMovementInput에 안들어가짐
	3. Normalize는 직접 변환시키는 방식 / GetSafeNormal은 변환후 넘기는 방식
	4. AddMovementInput -> 캐릭터 이동 / p = p0+vt
	5. UE_LOG(카테고리, 색상 or Log,Warning,Error,Fatal 표시, Fatal은 에디터를 크래시, TEXT)
	6. AddControllerYawInput -> 캐릭터 좌우 회전 및 컨트롤러 기반 회전

	*/

	Super::Tick(DeltaTime);

	moveDir.Normalize();
		

	//바라보는 방향으로 이동
	//moveDir = pc->GetControlRotation() * moveDir.X + pc->GetControlRotation() * moveDir.Y;
	FVector forwardvector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightvector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);
	moveDir = (forwardvector * moveDir.X + rightvector * moveDir.Y).GetSafeNormal(); //2 벡터를 계산하고 safenormal로 변환된 값을 moveDir로 넣는다.

	//이동속도는 뷰포트에서 캐릭터 MoveMent 컴포넌트에서 Walking에 있는 Max speed 조절
	AddMovementInput(moveDir);

	

	//Log 출력
	//UE_LOG(LogTemp, Warning, TEXT("Yaw: %f, Pitch: %f"),rotateAxis.Yaw, rotateAxis.Pitch);

	//캐릭터 좌우 회전 - 플레이어 컨트롤러 회전 기반
	AddControllerYawInput(rotateAxis.Yaw);
	
	//캐릭터 상하 회전 - 스프링 암 회전 기반
	FRotator curCamRot = SpringArmComp->GetComponentRotation(); // 현재 카메라 로테이션은 스프링 암의 로테이션 컴포넌트를 가져옴
	
	float modifyPitch = curCamRot.Pitch + rotateAxis.Pitch;
	//curCamRot + FRotator(rotateAxis.Pitch, 0, 0); -> 수정된 로테이션은 현재로테이션 + 마우스 Pitch 방향
	//아래는 스프링암이 각도를 넘어가지 않게 -60~60도로 제한
	modifyPitch = FMath::Clamp(modifyPitch, -60, 60);
	FRotator ModifiedRot = FRotator(modifyPitch, curCamRot.Yaw, curCamRot.Roll);

	SpringArmComp->SetWorldRotation(ModifiedRot); //수정된 로테이션을 SetWroldRotation으로 함.

	

}

// Called to bind functionality to input
void AMy_char::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//기존 입력 이벤트 변수를 EnhancdeInput 형태로 캐스팅 함.
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);


	// 입력 키와 실행할 함수를 연결Bind 하기
	if (enhancedInputComponent != nullptr)
	{
		//Jump
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping); // 이미 만들어져있는거기에 부모 클래스인 ACharacter

		//Move
		enhancedInputComponent->BindAction(ia_move, ETriggerEvent::Triggered, this, &AMy_char::OnMoveInput); // 우리가 만든거기에 Mychar
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
	// 키 입력 이벤트 발생 시에 받은 value 값을 Fvector2D형태로 변환

	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}


void AMy_char::OnRotateInput(const FInputActionValue& value)
{
	//yaw 축은 (좌우) pitch는 (상하)
	//Fvector2D로 받아서 변환하기 때문에 아래처럼함.
	FVector2D rotateInput = value.Get<FVector2D>();

	rotateAxis.Pitch = rotateInput.Y;
	rotateAxis.Yaw = rotateInput.X;

	//블루프린트 방식

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
	
	//1. 블루프린트 파일 생성
	/*
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	*/
	


	//2. 코드로 생성

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	/*
	if(spawnBullet != nullptr)
	{
		spawnBullet->StaticMeshComp->SetWorldScale3D(FVector(1f, 1f, 1f)); 이것도 가능함

	}
	*/
	



}

