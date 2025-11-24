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

	// 무기가 부여하는 어빌리티(사격, 재장전, 특수능력 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ability")
	TObjectPtr<UGRAbilitySet> AbilitySet;
};
