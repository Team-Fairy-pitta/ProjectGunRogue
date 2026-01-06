// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GRAICharacter.generated.h"

class UGameplayAbility;
class UGRZLocationComponent;
class UGRHealthAttributeSet;
class UGRCombatAttributeSet;
class AGRGoodsActor;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FDropGoodsInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGRGoodsActor> GoodsClass;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;

	UPROPERTY(EditAnywhere)
	int32 RandomRange = 2;

	UPROPERTY(EditAnywhere)
	float DropChance = 1.0f;
};

UCLASS()
class GUNROGUE_API AGRAICharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGRAICharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReapon) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRZLocationComponent> ZLocationComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities",meta=(AllowPrivateAccess))
	TArray<TSubclassOf<UGameplayAbility>> AttackAbilityClassList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttributeSet")
	TObjectPtr<UGRHealthAttributeSet> HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttributeSet")
	TObjectPtr<UGRCombatAttributeSet> CombatAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AttributeSet|InitStatus")
	float InitMaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AttributeSet|InitStatus")
	float InitMaxShield;

	bool bIsDead = false;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnDead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OnDead();

	void OnDead_ProcessAuth();
	void OnDead_ProcessNormal();

private:
	void InitAbilitySystemComponent();
	void InitStatus();

	void NotifySpawnToGameMode();
	void NotifyDestroyToGameMode();

#pragma region Drops
protected:
	void DropGoods();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DropGoods")
	TArray<FDropGoodsInfo> DropGoodsList;

private:
	void DropGoodsForEachPlayer(APlayerController* Player);
	TArray<FDropGoodsInfo> GetDropGoodsList();
	void SpawnToTargetPlayer(APlayerState* InPlayerState, TSubclassOf<AGRGoodsActor> GoodsClass, int32 DropCount);

	FVector GetRandomOffsetAround() const;
	FVector GetGroundLocation(const FVector& InXY) const;

	bool CanDropGoods(float Chance) const;
	int32 GetDropCount(int32 BaseCount, int32 RandomRange) const;
	
#pragma endregion
};
