#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFireComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_001_API UPlayerFireComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerFireComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class AGranadeActor> Granade_bp;

	UPROPERTY(EditAnywhere, Category = MySetting)
	TSubclassOf<class UCameraShakeBase> Shake_bp;




	UPROPERTY(EditAnywhere, Category = MySetting)
	class UParticleSystem* hit_fx;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class USoundBase* fire_sound;

	UPROPERTY(EditAnywhere, Category = MySetting)
	class UAnimMontage* Fire_motage;

	
	void OnFireInput(const struct FInputActionValue& value);
	void ONThrow();

	UPROPERTY(EditAnywhere, Category = MySetting)
	int32 atkdmg = 1;

private:
	class AMy_char* player;




		
};
