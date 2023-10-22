// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"

UCLASS()
class TEST_001_API ABulletActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = MySettings)
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = MySettings)
	class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	float MoveSpeed = 800.0f;

};
