#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "Item/GRItemActor.h"
#include "Weapon/GRWeaponHandle.h"
#include "GRPlayerState.generated.h"

class AGRPlayerController;
class AGRCharacter;
class UGRAbilitySystemComponent;
class UGRWeaponDefinition;
class AGRWeaponActor;
struct FGameplayEffectSpec;
struct FGRWeaponInstance;

DECLARE_MULTICAST_DELEGATE(FOnAbilitySystemComponentInit);

DECLARE_MULTICAST_DELEGATE(FOnWeaponDataUpdata);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipped, int32, SlotIndex, UGRWeaponDefinition*, WeaponDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponDropped, int32, SlotIndex, UGRWeaponDefinition*, WeaponDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSwitched, int32, OldSlotIndex, int32, NewSlotIndex);

namespace WeaponSlot
{
	constexpr int32 MaxWeaponSlots = 2;  // 무기 슬롯 개수
	constexpr int32 FirstSlot = 0;     // 1번 슬롯 (인덱스 0)
	constexpr int32 SecondarySlot = 1;   // 2번 슬롯 (인덱스 1)
}

UCLASS()
class GUNROGUE_API AGRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGRPlayerState();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRCharacter* GetGRCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	UGRAbilitySystemComponent* GetGRAbilitySystemComponent();

	// 무기 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponDropped OnWeaponDropped;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponSwitched OnWeaponSwitched;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FOnAbilitySystemComponentInit OnAbilitySystemComponentInit;

	bool IsAbilitySystemComponentInit() const { return bIsAbilitySystemComponentInit; }

	UFUNCTION(BlueprintCallable)
	bool HasItem(UGRItemDefinition* ItemDefinition);

	UFUNCTION(BlueprintCallable)
	void TryEquipItem(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(BlueprintCallable)
	void UnequipItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetItemNum();

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void TryEquipWeapon(UGRWeaponDefinition* WeaponDefinition, FGRWeaponInstance& Instance, AActor* WeaponActor);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void DropWeapon(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void DropCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void SwitchWeapon(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	bool HasWeaponInSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	int32 GetCurrentWeaponSlotIndex() const { return CurrentWeaponSlot; }

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	UGRWeaponDefinition* GetWeaponDefinitionInSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	UGRWeaponDefinition* GetCurrentWeaponDefinition() const;

	FGRWeaponInstance* GetWeaponInstanceInSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipItemActor(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UnequipItemActor(int32 ItemIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance, AActor* WeaponActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_DropWeapon(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SwitchWeapon(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void UpgradeWeapon(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UpgradeWeapon(int32 SlotIndex);

	UFUNCTION()
	void OnRep_WeaponDataUpdata();

	UFUNCTION()
	void AllRerollOptionWeapon(int32 InWeaponSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_AllRerollOptionWeapon(int32 InWeaponSlotIndex);

	UFUNCTION()
	void RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex);

	FOnWeaponDataUpdata OnWeaponDataUpdata;

	TArray<FGRItemHandle>& GetItemHandles() { return ItemHandles; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Class")
	TSubclassOf<AGRItemActor> ItemActorClass;

	UPROPERTY(VisibleAnywhere, Category = "GRPlayerState|AbilitySystemComponent")
	TObjectPtr<UGRAbilitySystemComponent> AbilitySystemComponent;

	FGRAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY(Replicated)
	TArray<FGRItemHandle> ItemHandles;

	UPROPERTY()
	TSet<UGRItemDefinition*> ItemDefinitionSet;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponDataUpdata)
	TArray<FGRWeaponHandle> WeaponSlots;

	UPROPERTY(Replicated)
	int32 CurrentWeaponSlot = -1; // -1은 무기 없음

private:
	UFUNCTION()
	void OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	void InitAbilitySystemComponent();

	void OnEquipItem(UGRItemDefinition* ItemDefinition);
	void OnUnequipItem(UGRItemDefinition* ItemDefinition);
	void DropWeaponAtPlayerFront(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance);

	FVector GetGroundPointUsingLineTrace(AActor* SpawnedActor);
	void PlaceActorOnGround(AActor* SpawnedActor);

	// 무기 헬퍼 함수
	int32 FindEmptyWeaponSlot() const;
	void ActivateWeaponInSlot(int32 SlotIndex);
	void DeactivateWeaponInSlot(int32 SlotIndex);
	void SpawnWeaponAtLocation(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& WeaponInstance, const FVector& Location, const FRotator& Rotation);

	bool bIsAbilitySystemComponentInit = false;
};
