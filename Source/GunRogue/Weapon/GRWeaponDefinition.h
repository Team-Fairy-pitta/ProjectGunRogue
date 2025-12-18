#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/GRWeaponTypes.h"
#include "Character/Attachment/GRCharacterAttachment.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "GRWeaponDefinition.generated.h"

class UTexture2D;
class UStaticMesh;
class USkeletalMesh;
class UAnimSequence;
class UAnimMontage;
class UGRAbilitySet;
class UGRWeaponOptionPool;
class USoundBase;
class AGRProjectile;
class UGameplayEffect;

UCLASS()
class GUNROGUE_API UGRWeaponDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// ======== 기본 정보 ========
	// 무기 카테고리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	EGRWeaponCategory WeaponCategory = EGRWeaponCategory::None;

	// 무기 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	FText WeaponName;

	// 무기 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	FText WeaponDescription;

	// 무기 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	TObjectPtr<UTexture2D> WeaponIcon;

	// ======== 발사 타입 ========
	// 무기 발사 방식
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|FireType")
	EGRWeaponFireType FireType = EGRWeaponFireType::Hitscan;

	// ======== 투사체 설정 ========
	// 투사체 클래스 (FireType이 Projectile일 때 사용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile",
	meta = (EditCondition = "FireType == EGRWeaponFireType::Projectile", EditConditionHides))
	TSubclassOf<AGRProjectile> ProjectileClass;

	// 투사체 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile",
		meta = (EditCondition = "FireType == EGRWeaponFireType::Projectile", EditConditionHides))
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile",
	meta = (EditCondition = "FireType == EGRWeaponFireType::Projectile", EditConditionHides))
	float ProjectileSpeed = 3000.0f;

	// 투사체 중력 스케일 (0 = 중력 무시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile",
	meta = (EditCondition = "FireType == EGRWeaponFireType::Projectile", EditConditionHides))
	float ProjectileGravityScale = 0.0f;

	// 투사체 수명 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile",
		meta = (EditCondition = "FireType == EGRWeaponFireType::Projectile", EditConditionHides))
	float ProjectileLifeSpan = 10.0f;

	// ======== 메시 ========
	// 무기 메시 (바닥에 떨어져 있을 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<UStaticMesh> WeaponPickupMesh;

	// 장착했을 때, 캐릭터에 붙여야 하는 Attachment 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	FGRCharacterAttachment WeaponAttachment;

	// ======== 어빌리티 ========
	// 무기가 부여하는 어빌리티(사격, 재장전, 특수능력 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ability")
	TObjectPtr<UGRAbilitySet> AbilitySet;

	// ======== 애니메이션 ========
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimSequence> IdleAnimSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> EquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> ReloadAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> FireAnimMontage;

	// ======== 사운드 ========
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> EmptyFireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> ImpactSound;


	// ======== 이펙트 ========
	// 머즐 플래시 - Niagara (우선)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UNiagaraSystem> MuzzleFlashNiagara;

	// 머즐 플래시 - Cascade (Niagara 없을 때 사용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UParticleSystem> MuzzleFlashCascade;

	// 히트 이펙트 - Niagara
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UNiagaraSystem> ImpactEffectNiagara;

	// 히트 이펙트 - Cascade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UParticleSystem> ImpactEffectCascade;

	// 총알 궤적 (Tracer) - Niagara
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UNiagaraSystem> BulletTracerNiagara;

	// 총알 궤적 (Tracer) - Cascade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effect")
	TObjectPtr<UParticleSystem> BulletTracerCascade;

	// ======== 무기 스탯 ========
	/* 무기 강화 파트에서 사용하고 있는 값 */
	/* [NOTE] 무기의 Status Effect 랑 어떻게 통합할 수 있을까..? */
	// 무기 기본 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	float BaseDamage = 10.f;

	// 무기 최대 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	int32 MaxLevel = 15;

	// 무기 레벨업 당 증가하는 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	float UpgradeDamageIncrease = 2.f;

	// 무기 옵션 풀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Basic")
	UGRWeaponOptionPool* OptionPool = nullptr;

	// 탄창 크기 (최대 탄약)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo = 30;

};
