#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRWeaponInstance.generated.h"

class UGRAbilitySystemComponent;
class UGRWeaponDefinition;
class USkeletalMeshComponent;

/*
무기 인스턴스 핸들
플레이어가 장착한 무기의 런타임 정보를 관리.
 */
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRWeaponInstance
{
	GENERATED_BODY()

public:
	// 무기 장착
	void EquipWeapon(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition);

	// 무기 해제
	void UnequipWeapon();

	// 장착된 무기인지 확인
	bool IsEquipped() const { return WeaponDefinition != nullptr; }

	// 무기 정의 가져오기
	UGRWeaponDefinition* GetWeaponDefinition() const { return WeaponDefinition; }

private:
	// 부여된 어빌리티/이펙트 핸들
	UPROPERTY()
	FGRAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveEffectHandles;

	// 캐시된 ASC
	UPROPERTY()
	TObjectPtr<UGRAbilitySystemComponent> CachedASC;

	// 무기 정의
	UPROPERTY()
	TObjectPtr<UGRWeaponDefinition> WeaponDefinition;
};
