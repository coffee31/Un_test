

#include "PlayerFireComp.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "GranadeActor.h"
#include "Components/SphereComponent.h"
#include "My_char.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UPlayerFireComp::UPlayerFireComp()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UPlayerFireComp::BeginPlay()
{
	Super::BeginPlay();


	//컴포넌트가 부착된 액터
	player = Cast<AMy_char>(GetOwner());
	
}


// Called every frame
void UPlayerFireComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UPlayerFireComp::OnFireInput(const FInputActionValue& value)
{
	if (player == nullptr) return;

	if (player->GetCurrentHP() <= 0)
	{
		return;
	}
	// 기본 셋팅
	//FVector startLoc = GunMeshComp->GetSocketLocation(FName("Muzzle"));
	//FVector endLoc = startLoc + GunMeshComp->GetRightVector() * 1000.0f;

	FVector startLoc = player->CameraComp->GetComponentLocation() + player->CameraComp->GetForwardVector() * player->SpringArmComp->TargetArmLength;
	FVector endLoc = startLoc + player->CameraComp->GetForwardVector() * 3000.0f;

	FCollisionQueryParams params;
	params.AddIgnoredActor(player);

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
		AEnemy* enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			enemy->OnDamager(atkdmg);
			UE_LOG(LogTemp, Warning, TEXT("current enemy hp : %d"), enemy->GetCurrentHP());


			/*
			if (mainwidget_inst != nullptr && enemy->GetCurrentHP() <= 0)
			{
				mainwidget_inst->AddScore(1);
			}
			*/
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
	player->PlayAnimMontage(Fire_motage);

}

void UPlayerFireComp::ONThrow()
{
	if (player == nullptr) return;

	if (player->GetCurrentHP() <= 0)
	{
		return;
	}
	//1. 수류탄 생성
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGranadeActor* grande_inst = GetWorld()->SpawnActor<AGranadeActor>(Granade_bp, player->GetActorLocation() + player->GetActorForwardVector() * 100, FRotator::ZeroRotator, params);
	if (grande_inst != nullptr)
	{
		// 수류탄 엑트를 던질 수 있는 상태로 변경
		grande_inst->SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		grande_inst->SphereCollision->SetSimulatePhysics(true);
		grande_inst->SphereCollision->SetEnableGravity(true);
		FVector Throwdir = (player->GetActorForwardVector() + player->GetActorUpVector()).GetSafeNormal();
		grande_inst->SphereCollision->AddImpulse(Throwdir * 1500);
	}


	//2. 수류탄 대각선 45도로 발사
}