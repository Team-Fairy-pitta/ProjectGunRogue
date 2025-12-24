#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GRBladeWaveProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGRGameplayEffect;

UCLASS()
class GUNROGUE_API AGRBladeWaveProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGRBladeWaveProjectile();

	void InitProjectile(float InDamage, float InWaveScale, bool bInPierce);

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Damage")
	TSubclassOf<UGRGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Damage")
	FGameplayTag DataTag_Damage;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Life")
	float LifeSeconds = 3.0f;

	float Damage = 0.f;
	float WaveScale = 1.f;
	bool bPierce = false;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void ApplyDamageTo(AActor* TargetActor, const FHitResult& Hit) const;

};
