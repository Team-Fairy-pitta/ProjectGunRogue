#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/GRWeaponTypes.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRWeaponDefinition.generated.h"

class UTexture2D;
class UStaticMesh;
class USkeletalMesh;

UCLASS()
class GUNROGUE_API UGRWeaponDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
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

	// 무기 메시 (바닥에 떨어져 있을 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<UStaticMesh> WeaponPickupMesh;

	// 무기 스켈레탈 메시 (장착했을 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<USkeletalMesh> WeaponEquippedMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	TArray<TSubclassOf<UGameplayEffect>> WeaponEffects;

	// 무기 스탯 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	TSubclassOf<UGameplayEffect> WeaponStatsEffect;

	// 무기가 부여하는 어빌리티(특수 능력 있는 무기용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ability")
	TObjectPtr<UGRAbilitySet> AbilitySet;

	/* 오류로 제외
	// 무기 기본 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float BaseDamage = 10.0f;

	// 무기 기본 약점 배율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float BaseCriticalMultiplier = 2.0f;

	// 무기 강화 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	int32 EnhancementLevel = 0;*/
};
