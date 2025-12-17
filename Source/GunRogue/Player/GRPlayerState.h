#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "Item/GRItemActor.h"
#include "Character/Attachment/GRCharacterAttachment.h"
#include "Weapon/GRWeaponHandle.h"
#include "Augment/GRAugmentStructs.h"
#include "MetaProgression/GRPerkStructs.h"
#include "GRPlayerState.generated.h"

class AGRPlayerController;
class AGRCharacter;
class UGRAbilitySystemComponent;
class UGameplayEffect;
class UGRHealthAttributeSet;
class UGRCombatAttributeSet;
class UGRWeaponDefinition;
class AGRWeaponActor;
class UGRAugmentDefinition;
struct FGRWeaponInstance;


DECLARE_MULTICAST_DELEGATE(FOnAbilitySystemComponentInit);

DECLARE_MULTICAST_DELEGATE(FOnWeaponDataUpdata);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipped, int32, SlotIndex, UGRWeaponDefinition*, WeaponDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponDropped, int32, SlotIndex, UGRWeaponDefinition*, WeaponDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSwitched, int32, OldSlotIndex, int32, NewSlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAugmentChanged, FName, AugmentID, int32, NewLevel);

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
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void CopyProperties(class APlayerState* PlayerState) override;

	UFUNCTION(BlueprintCallable, Category = "GRPlayerState")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "GRPlayerState")
	AGRCharacter* GetGRCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "GRPlayerState")
	UGRAbilitySystemComponent* GetGRAbilitySystemComponent();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool IsAbilitySystemComponentInit() const { return bIsAbilitySystemComponentInit; }

	FOnAbilitySystemComponentInit OnAbilitySystemComponentInit;
  
protected:
	UPROPERTY(VisibleAnywhere, Category = "GRPlayerState|AbilitySystemComponent")
	TObjectPtr<UGRAbilitySystemComponent> AbilitySystemComponent;

	FGRAbilitySet_GrantedHandles GrantedHandles;

private:
	UFUNCTION()
	virtual void OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	void InitAbilitySystemComponent();

	FVector GetGroundPointUsingLineTrace(AActor* SpawnedActor);
	void PlaceActorOnGround(AActor* SpawnedActor);

	bool bIsAbilitySystemComponentInit = false;

#pragma region Item
public:
	UFUNCTION(BlueprintCallable)
	bool HasItem(UGRItemDefinition* ItemDefinition);

	UFUNCTION(BlueprintCallable)
	void TryEquipItem(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(BlueprintCallable)
	void UnequipItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetItemNum();

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipItemActor(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UnequipItemActor(int32 ItemIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Class")
	TSubclassOf<AGRItemActor> ItemActorClass;

	UPROPERTY(Replicated)
	TArray<FGRItemHandle> ItemHandles;

	UPROPERTY()
	TSet<UGRItemDefinition*> ItemDefinitionSet;

private:
	void OnEquipItem(UGRItemDefinition* ItemDefinition);
	void OnUnequipItem(UGRItemDefinition* ItemDefinition);
#pragma endregion

#pragma region Weapon
public:
	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponDropped OnWeaponDropped;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Weapon|Events")
	FOnWeaponSwitched OnWeaponSwitched;

	UFUNCTION(Client, Reliable)
	void ClientRPC_BroadcastOnWeaponEquipped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition);

	UFUNCTION(Client, Reliable)
	void ClientRPC_BroadcastOnWeaponDropped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition);

	UFUNCTION(Client, Reliable)
	void ClientRPC_BroadcastOnWeaponSwitched(int32 OldSlotIndex, int32 NewSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance, AActor* WeaponActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_DropWeapon(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SwitchWeapon(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UpgradeWeapon(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_AllRerollOptionWeapon(int32 InWeaponSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_PlayWeaponEquipAnimMontage();

	void UpdateWeaponAttachToCharacter();

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
	void UpgradeWeapon(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	int32 GetCurrentWeaponSlotIndex() const { return CurrentWeaponSlot; }

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	UGRWeaponDefinition* GetWeaponDefinitionInSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	UGRWeaponDefinition* GetCurrentWeaponDefinition() const;

	FGRWeaponInstance* GetWeaponInstanceInSlot(int32 SlotIndex);

	FGRWeaponHandle* GetActiveWeaponHandle();

	UFUNCTION()
	void OnRep_WeaponDataUpdata();

	UFUNCTION()
	void AllRerollOptionWeapon(int32 InWeaponSlotIndex);

	UFUNCTION()
	void RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex);

	FOnWeaponDataUpdata OnWeaponDataUpdata;

	TArray<FGRItemHandle>& GetItemHandles() { return ItemHandles; }

	FGRCharacterAttachmentHandle GetCurrentWeaponAttachmentHandle() const { return CurrentWeaponAttachmentHandle; }


protected:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponDataUpdata)
	TArray<FGRWeaponHandle> WeaponSlots;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponSlot)
	int32 CurrentWeaponSlot = INDEX_NONE;

	UFUNCTION()
	void OnRep_CurrentWeaponSlot();

	UPROPERTY()
	FGRCharacterAttachmentHandle CurrentWeaponAttachmentHandle;

private:
	// 클라이언트 예측용 로컬 슬롯
	int32 PredictedWeaponSlot = INDEX_NONE;
	int32 PreviousWeaponSlot = INDEX_NONE;

	int32 FindEmptyWeaponSlot() const;

	void ActivateWeaponInSlot(int32 SlotIndex);
	void DeactivateWeaponInSlot(int32 SlotIndex);
	void SpawnWeaponAtLocation(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& WeaponInstance, const FVector& Location, const FRotator& Rotation);

	bool IsValidSlotIndex(int32 SlotIndex) const;
	void EquipWeaponToSlot(int32 SlotIndex, UGRWeaponDefinition* WeaponDef, const FGRWeaponInstance& Instance);
	void UnequipWeaponFromSlot(int32 SlotIndex);
	void SwitchToSlot(int32 NewSlotIndex);
	bool TrySwitchToOtherWeapon(int32 ExcludeSlotIndex);
	void UpdateCurrentWeaponAmmoDisplay();
	void ResetAmmoDisplay();

	void DropWeaponAtPlayerFront(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance);

#pragma endregion

#pragma region Augment
public:
	UPROPERTY(ReplicatedUsing = OnRep_OwnedAugments)
	TArray<FAugmentEntry> OwnedAugments;
	
	UPROPERTY(BlueprintAssignable)
	FAugmentChanged OnAugmentChanged;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnAugmentSelected(FName AugmentID);

	int32 GetAugmentLevel(FName AugmentID);
	
	UFUNCTION()
	void OnRep_OwnedAugments();

protected:
	void AddAugment(FName AugmentID);
	void LevelUpAugment(int32 Index);
	
	TArray<FAugmentEntry> PreviousOwnedAugments;
	
#pragma endregion

#pragma region Perk;
protected:
	virtual void InitPerkFromSave();
	void InitPerkInfoRows();
	void LoadPerkFromSave(const TArray<FPerkEntry>& LoadedPerkInfoRows, int32 LoadedMetaGoods);
	virtual void SavePerkToSave();
	void InitPlayerID();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetCurrentMetaGoods(int32 InMetaGoods);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyAllPerksToASC(const TArray<FPerkEntry>& PerkInfos);

	FString PlayerID;

	UPROPERTY(EditAnywhere, Category="Perk")
	TSubclassOf<UGameplayEffect> PerkGE;

	TArray<FPerkEntry> PerkInfoRows;

#pragma endregion

#pragma region Goods;
public:
	void AddMetaGoods(int32 Amount);
	void AddGold(int32 Amount);

	int32 GetCurrentMetaGoods() const { return CurrentMetaGoods; }
	int32 GetGold() const { return Gold; }
	
	void UpdateMetaGoodsUI();
	void UpdateGoldUI();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMetaGoods)
	int32 CurrentMetaGoods; // 가지고 있던 재화 + 게임에서 얻은 재화

	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	int32 Gold;

private:
	UFUNCTION()
	void OnRep_CurrentMetaGoods();

	UFUNCTION()
	void OnRep_Gold();
	
#pragma endregion
};


