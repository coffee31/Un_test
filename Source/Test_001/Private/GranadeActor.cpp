// Fill out your copyright notice in the Description page of Project Settings.


#include "GranadeActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGranadeActor::AGranadeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(50.0f);
	SphereCollision->SetWorldScale3D(FVector(0.15f));							
	SphereCollision->SetMassOverrideInKg(NAME_None, 0.5f, true);			// ���� ����
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
	
}

// Called every frame
void AGranadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

