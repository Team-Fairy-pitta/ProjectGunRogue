#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestTurret.generated.h"

class ATestProjectile;

UCLASS()
class GRTEST_API ATestTurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestTurret();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	// 발사할 Projectile 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret")
	TSubclassOf<ATestProjectile> ProjectileClass;

	// 발사 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float FireInterval = 2.0f;

private:
	float TimeSinceLastFire = 0.0f;

	void FireProjectile();

};
