#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "Weapon/GRWeaponInstance.h"
#include "GRWeaponHandle.generated.h"

class UGRAbilitySystemComponent;
class UGRWeaponDefinition;
struct FGRWeaponInstance;

/*
무기 인스턴스 핸들
플레이어가 장착한 무기의 런타임 정보를 관리.
 */
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRWeaponHandle
{
	GENERATED_BODY()

public:
	// 무기 장착
	void EquipWeapon(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance);

	// 무기 해제
	void UnequipWeapon();

	// 무기 활성화 (Effect 적용)
	void ActivateWeapon();

	// 무기 비활성화 (Effect 제거, 슬롯에는 유지)
	void DeactivateWeapon();

	// 장착된 무기인지 확인
	bool IsEquipped() const { return WeaponDefinition != nullptr; }

	// 무기 정의 가져오기
	UGRWeaponDefinition* GetWeaponDefinition() const { return WeaponDefinition; }

	FGRWeaponInstance GetWeaponInstanceCopy() { return WeaponInstance; }
	
	FGRWeaponInstance* GetWeaponInstanceRef() { return &WeaponInstance; }

	bool IsActive() const { return bIsActive; }

private:
	// 부여된 어빌리티/이펙트 핸들
	UPROPERTY()
	FGRAbilitySet_GrantedHandles GrantedHandles;

	// 캐시된 ASC
	UPROPERTY()
	TObjectPtr<UGRAbilitySystemComponent> CachedASC;

	// 무기 정의
	UPROPERTY()
	TObjectPtr<UGRWeaponDefinition> WeaponDefinition;

	UPROPERTY()
	FGRWeaponInstance WeaponInstance;

	bool bIsActive = false;
};
