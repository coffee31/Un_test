// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "Components/SphereComponent.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(SphereComp);

	SphereComp->SetSphereRadius(6.4f);


	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Comp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector dir = GetActorForwardVector() * MoveSpeed;

	SetActorLocation(GetActorLocation() + dir * DeltaTime);

}

