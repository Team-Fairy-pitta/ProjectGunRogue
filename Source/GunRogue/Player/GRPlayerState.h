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

DECLARE_MULTICAST_DELEGATE(FOnAbilitySystemComponentInit);

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
	void TryEquipWeapon(UGRWeaponDefinition* WeaponDefinition, AActor* WeaponActor);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	void DropWeapon();

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	bool HasWeaponEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Weapon")
	UGRWeaponDefinition* GetEquippedWeaponDefinition() const;

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipItemActor(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UnequipItemActor(int32 ItemIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon(UGRWeaponDefinition* WeaponDefinition, AActor* WeaponActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_DropWeapon();

protected:
	UPROPERTY(VisibleAnywhere, Category = "ITPlayerState|AbilitySystemComponent")
	TObjectPtr<UGRAbilitySystemComponent> AbilitySystemComponent;

	FGRAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY(Replicated)
	TArray<FGRItemHandle> ItemHandles;

	UPROPERTY()
	TSet<UGRItemDefinition*> ItemDefinitionSet;

	UPROPERTY(Replicated)
	FGRWeaponHandle EquippedWeapon;

private:
	UFUNCTION()
	void OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	void InitAbilitySystemComponent();

	void OnEquipItem(UGRItemDefinition* ItemDefinition);
	void OnUnequipItem(UGRItemDefinition* ItemDefinition);

	FVector GetGroundPointUsingLineTrace(AActor* SpawnedActor);
	void PlaceActorOnGround(AActor* SpawnedActor);

	// 무기 스폰 헬퍼 함수
	void SpawnWeaponAtLocation(UGRWeaponDefinition* WeaponDefinition, const FVector& Location, const FRotator& Rotation);

	bool bIsAbilitySystemComponentInit = false;
};
