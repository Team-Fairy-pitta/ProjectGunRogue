#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GRBladeWaveProjectile.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;
class UGRGameplayEffect;
class UGRAbilitySystemComponent;
class UGRSkillAttributeSet_MeleeSkill;

UCLASS()
class GUNROGUE_API AGRBladeWaveProjectile : public AActor
{
	GENERATED_BODY()

public:
	AGRBladeWaveProjectile();

	void InitProjectile(float InDamage, float InWaveScale, bool bInPierce);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* Root = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCapsuleComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Damage")
	TSubclassOf<UGRGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Damage")
	FGameplayTag DataTag_Damage;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Life")
	float LifeSeconds = 3.0f;

	UPROPERTY(Replicated)
	float Damage = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_WaveScale)
	float WaveScale = 1.f;

	UPROPERTY(Replicated)
	bool bPierce = false;

	UFUNCTION()
	void OnRep_WaveScale();

private:
	TSet<TWeakObjectPtr<AActor>> HitActors;

	void ComputeParametersOnServer();

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
