// Fill out your copyright notice in the Description page of Project Settings.


#include "GranadeActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGranadeActor::AGranadeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(50.0f);
	SphereCollision->SetWorldScale3D(FVector(0.15f));							
	//SphereCollision->SetMassOverrideInKg(NAME_None, 0.5f, true);			// ���� ����
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// �浹 ó�� off
	SphereCollision->SetSimulatePhysics(false);								// ���� ���� off
	SphereCollision->SetEnableGravity(false);								// �߷� ���� off



	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

// Called when the game starts or when spawned
void AGranadeActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Sphere Collision�� �浹�� ���� üũ �� ������ �Լ� �����ϵ��� ����
	// Delegate
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AGranadeActor::OnOverlap);

}

// Called every frame
void AGranadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGranadeActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* ���� ��󿡰Ը� ������
		AEnemy* enemy =  Cast<AEnemy>(OtherActor);

		if (enemy != nullptr)
		{
			enemy->OnDamager(ExplosionDamage);
		}
	*/

	//2. �ݰ� ���� �� ��� ������ ������
	TArray<FOverlapResult> hitinfos;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_Pawn);
	

	bool bIsOverlap = GetWorld()->OverlapMultiByObjectType(hitinfos, GetActorLocation(), GetActorRotation().Quaternion(), params, FCollisionShape::MakeSphere(800));

	if (bIsOverlap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Count : %d"), hitinfos.Num());
		for (FOverlapResult& hitinfo : hitinfos)
		{
			AEnemy* enemy = Cast<AEnemy>(hitinfo.GetActor());	
			if (enemy != nullptr)
			{
				enemy->OnDamager(ExplosionDamage);
				UE_LOG(LogTemp, Warning, TEXT("Hit Name : %s"), *enemy->GetActorNameOrLabel());
			}
			Destroy();
		}
	}

	//�浹 ���� �ݰ� ǥ��
	DrawDebugSphere(GetWorld(), GetActorLocation(), 800, 25, FColor::Green, false, 5.0f, 0, 2);

}

