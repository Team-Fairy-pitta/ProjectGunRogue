#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class AGRCharacter;
class UGameplayEffect;

/**
 * 투사체 베이스 클래스
 * - 폭발 범위가 있으면 범위 데미지
 * - 폭발 범위가 0이면 직격 데미지만
 */

UCLASS()
class GUNROGUE_API AGRProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AGRProjectile();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// 투사체 초기화 (발사 시 호출)
	void InitializeProjectile(
		AGRCharacter* InOwnerCharacter,
		float InDamage,
		float InExplosionRadius,
		float InExplosionFalloff,
		const FVector& InVelocity,
		float InGravityScale,
		float InLifeSpan,
		TSubclassOf<UGameplayEffect> InDamageEffect,
		UNiagaraSystem* InExplosionEffectNiagara = nullptr,
		UParticleSystem* InExplosionEffectCascade = nullptr,
		USoundBase* InExplosionSound = nullptr
	);

protected:
	// ======== 컴포넌트 ========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> TrailEffect;

	// ======== 투사체 외형 설정 ========
	// 투사체 메시 - 외형
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Visual")
	TObjectPtr<UStaticMesh> ProjectileMesh;

	// 트레일 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Visual")
	TObjectPtr<UNiagaraSystem> TrailNiagaraSystem;

	// 충돌 크기 여유 배율 (메시 크기 * 이 값)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Collision")
	float CollisionSizeMultiplier = 1.2f;

private:
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyExplosionDamage(const FVector& ExplosionLocation);
	void ApplyDirectDamage(AActor* HitActor, const FHitResult& Hit);

	void PlayExplosionFX(const FVector& Location, float ExplosionScale);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionFX(const FVector& HitLocation, float ExplosionScale);

	// 메시 크기 기반 충돌 크기 계산
	void SetupCollisionFromMesh();

	UPROPERTY()
	TObjectPtr<AGRCharacter> OwnerCharacter;

	float Damage;
	float ExplosionRadius;
	float ExplosionFalloff;

	UPROPERTY(Replicated)
	bool bHasExploded = false;

	UPROPERTY(Replicated)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(Replicated)
	TObjectPtr<UNiagaraSystem> ExplosionEffectNiagara;

	UPROPERTY(Replicated)
	TObjectPtr<UParticleSystem> ExplosionEffectCascade;

	UPROPERTY(Replicated)
	TObjectPtr<USoundBase> ExplosionSound;
};
