// Fill out your copyright notice in the Description page of Project Settings.


#include "My_char.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "BulletActor.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GranadeActor.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "MainUserWidget.h"

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

	//캡슐 콜리전 프리셋 설정
	GetCapsuleComponent()->SetCollisionProfileName(FName("PlayerPreset"));


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

	UE_LOG(LogTemp, Warning, TEXT("%s(%d)Copile Test"),*FString(__FUNCTION__),__LINE__);
	UE_LOG(LogTemp, Warning, TEXT("RePushMain"));
	UE_LOG(LogTemp, Warning, TEXT("Time : today"));
	Hp = MaxHp;
	

	if (mainWidget != nullptr)
	{
		//위젯 블루프린트를 생성한다.
		mainwidget_inst = CreateWidget<UMainUserWidget>(GetWorld(), mainWidget);

		if (mainwidget_inst != nullptr)
		{
			//생성자에 위젯 인스턴스를 뷰 포트에 표시
			mainwidget_inst->AddToViewport();
		}
	}


	// Collision Respawn 변경(충돌 방식 변경) // 우리가 만든 프리셋은 config에 있음 코드에선 GameTraceChannel로 해야됨 -> 1234 등 순서는 config보고 맞추면됨
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	//GetCapsuleComponent()->SetCollisionEnabled() // 콜리전 상태변경





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


	//카메라를 fov값으로 서서히 변경
	CameraComp->FieldOfView = FMath::Lerp(CameraComp->FieldOfView, fov, DeltaTime * 5);

	

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
		enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Started, this, &AMy_char::DashON);
		enhancedInputComponent->BindAction(ia_Dash, ETriggerEvent::Completed, this, &AMy_char::DashOFF);

		//Crouch
		enhancedInputComponent->BindAction(ia_Crouch, ETriggerEvent::Started, this, &AMy_char::CrouchON);
		enhancedInputComponent->BindAction(ia_Crouch, ETriggerEvent::Completed, this, &AMy_char::CrouchOFF);

		//fire
		enhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Started, this, &AMy_char::OnFireInput);
		enhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Triggered, this, &AMy_char::OnFireInput);

		//Throw
		enhancedInputComponent->BindAction(ia_Thorw, ETriggerEvent::Completed, this, &AMy_char::ONThrow);

		//Zoom
		enhancedInputComponent->BindAction(ia_Zoom, ETriggerEvent::Started, this, &AMy_char::ZoomIn);
		enhancedInputComponent->BindAction(ia_Zoom, ETriggerEvent::Completed, this, &AMy_char::ZoomOut);
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
	GetCharacterMovement()->MaxWalkSpeed = 2400.0f;

}
void AMy_char::DashOFF()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

}

void AMy_char::CrouchON()
{
	Crouch();
	
}

void AMy_char::CrouchOFF()
{
	UnCrouch();
}

void AMy_char::ONThrow()
{
	//1. 수류탄 생성
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGranadeActor* grande_inst = GetWorld()->SpawnActor<AGranadeActor>(Granade_bp, GetActorLocation() + GetActorForwardVector() * 100, FRotator::ZeroRotator, params);
	if (grande_inst != nullptr)
	{
		// 수류탄 엑트를 던질 수 있는 상태로 변경
		grande_inst->SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		grande_inst->SphereCollision->SetSimulatePhysics(true);
		grande_inst->SphereCollision->SetEnableGravity(true);
		FVector Throwdir = (GetActorForwardVector() + GetActorUpVector()).GetSafeNormal();
		grande_inst->SphereCollision->AddImpulse(Throwdir * 500);
	}
	

	//2. 수류탄 대각선 45도로 발사
}

void AMy_char::ZoomIn()
{
	SpringArmComp->TargetArmLength = -75.0f;
	fov = 45.0f;
}

void AMy_char::ZoomOut()
{
	SpringArmComp->TargetArmLength = 500.0f;
	fov = 90.0f;
}


void AMy_char::OnFireInput(const FInputActionValue& value)
{
	// 기본 셋팅
	FVector startLoc = GunMeshComp->GetSocketLocation(FName("Muzzle"));
	FVector endLoc = startLoc + GunMeshComp->GetRightVector() * 1000.0f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

#pragma region 1. 블루프린트 총알 생성 및 발사
	//
	/*
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	*/

#pragma endregion

#pragma region 2. 코드로 총알 생성 및 발사
	//

	/*
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, GunMeshComp->GetSocketLocation(FName("Muzzle")), GunMeshComp->GetSocketRotation(FName("Muzzle")), params);

	if(spawnBullet != nullptr)
	{
		spawnBullet->StaticMeshComp->SetWorldScale3D(FVector(1f, 1f, 1f)); 이것도 가능함

	}
	*/
#pragma endregion

#pragma region 3. 라인트레이스로 발사하기 - 싱글 (라인트레이스는 월드 기준으로 발사하기때문에 월드 기준의 백터 사용)
	FHitResult hitInfo;
	bool bhit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECollisionChannel::ECC_Visibility, params);

	if (bhit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *hitInfo.GetActor()->GetActorNameOrLabel());
		AEnemy* enemy =  Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			enemy->OnDamager(atkdmg);
			UE_LOG(LogTemp, Warning, TEXT("current enemy hp : %d"), enemy->GetCurrentHP());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No hit"));
	}
	

#pragma endregion

#pragma region 4. 라인트레이스 멀티
/*
TArray<FHitResult> hitinfos;


bool bhit = GetWorld()->LineTraceMultiByChannel(hitinfos, startLoc, endLoc, ECC_Visibility, params);


if (bhit)
{
	for (FHitResult hitinfo : hitinfos)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hitinfo.GetActor()->GetActorNameOrLabel());

	}
	FVector hitLoc = hitinfos[0].ImpactPoint;

	DrawDebugLine(GetWorld(), startLoc, hitLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), hitLoc, endLoc, FColor::Green, false, 2.0f, 0, 1.0f);
	DrawDebugBox(GetWorld(), hitLoc, FVector(5), FQuat::Identity, FColor::Red, false, 2.0f, 0, 2.0f);


}
else
{
	//UE_LOG(LogTemp, Warning, TEXT("No hit"));
	DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Green, false, 2.0f, 0, 1.0f);
}
*/
#pragma endregion

#pragma region 5. 라인 트레이스 오브젝트 타입으로 비교
	/* 
	FHitResult hitinfo;
	FCollisionObjectQueryParams objparams;
	objparams.AddObjectTypesToQuery(ECC_WorldStatic); // 월드 스태틱만 체크하겠다.

	if (GetWorld()->LineTraceSingleByObjectType(hitinfo, startLoc, endLoc, objparams, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Object Name :  %s"), *hitinfo.GetActor()->GetActorNameOrLabel());
	}
	*/
#pragma endregion

#pragma region 6. 콜리전 프로필을 사용한 방법
	/*
	FHitResult hitinfo;
	if (GetWorld()->LineTraceSingleByProfile(hitinfo, startLoc, endLoc, FName("TestActor"), params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Object Name :  %s"), *hitinfo.GetActor()->GetActorNameOrLabel());

		// 충돌 지점에 특정 효과 생성
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_fx, hitinfo.ImpactPoint, FRotator::ZeroRotator, true);

	}
	*/
	

#pragma endregion
	UGameplayStatics::PlaySound2D(GetWorld(), fire_sound, 0.3f);
	/*
	// 카메라 쉐이킹

	if (pc != nullptr && Shake_bp != nullptr)
	{
		pc->ClientStartCameraShake(Shake_bp);
	}


	// 카메라 페이드

	pc->PlayerCameraManager->StartCameraFade(0, 1.0f, 1.0f, FLinearColor(1, 1, 1), true);
	*/

	// 총 발사 애니메이션 플레이
	PlayAnimMontage(Fire_motage);
	
}


void AMy_char::DamagePlayer(int32 damage)
{
	Hp = FMath::Max(0, Hp - damage);
}

//블루 프린트 노드로 사용할 함수
bool AMy_char::MyLineTraceMultiByChannel(TArray<FHitResult>& _hitinfos, const FVector _start, const FVector _end, ECollisionChannel _Ecc)
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bhit = GetWorld()->LineTraceMultiByChannel(_hitinfos, _start, _end, _Ecc, params);


	if (bhit)
	{
		FVector hitLoc = _hitinfos[0].ImpactPoint;

		DrawDebugLine(GetWorld(), _start, hitLoc, FColor::Red, false, 2.0f, 0, 1.0f);
		DrawDebugLine(GetWorld(), hitLoc, _end, FColor::Green, false, 2.0f, 0, 1.0f);
		DrawDebugBox(GetWorld(), hitLoc, FVector(5), FQuat::Identity, FColor::Red, false, 2.0f, 0, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), _start, _end, FColor::Green, false, 2.0f, 0, 1.0f);
	}



	return bhit;


}

