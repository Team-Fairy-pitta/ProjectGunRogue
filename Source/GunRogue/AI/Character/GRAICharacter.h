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
struct FOnAttributeChangeData;

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

	void OnHealthChanged(const FOnAttributeChangeData& Data);

private:
	void InitAbilitySystemComponent();
	void OnDead();

	void NotifySpawnToGameMode();
	void NotifyDestroyToGameMode();
};
