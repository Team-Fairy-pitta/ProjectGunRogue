#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/GRWeaponTypes.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "Weapon/GRWeaponOptionPool.h"
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

	// 무기가 부여하는 어빌리티(사격, 재장전, 특수능력 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ability")
	TObjectPtr<UGRAbilitySet> AbilitySet;

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

	
};
