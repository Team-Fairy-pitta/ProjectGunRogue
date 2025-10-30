#pragma once

#include "Engine/DataAsset.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GRAbilitySet.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UGRAbilitySystemComponent;

// [GameplayAbility, GameplayTag 묶음]
USTRUCT(BlueprintType)
struct FGRAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


// [Gameplay Effect]
USTRUCT(BlueprintType)
struct FGRAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 EffectLevel = 1;
};

// [Attribute Set]
USTRUCT(BlueprintType)
struct FGRAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};


// Pawn 또는 Character에게 부여한(grant) Ability, Effects, Attributes를 관리하기 위한 구조체
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	// Ability, Effect, AttributeSet을 부여함
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	// 부여한 모든 것(Ability, Effect, AttributeSet)을 제거함
	void TakeFromAbilitySystem(UGRAbilitySystemComponent* ASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


UCLASS()
class GUNROGUE_API UGRAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	// DataAsset에 설정해놓은 Ability, Effect, Attribute를 ASC에 적용한다.
	void GiveToAbilitySystem(UGRAbilitySystemComponent* ASC, FGRAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FGRAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect", meta = (TitleProperty = GameplayEffect))
	TArray<FGRAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Set", meta = (TitleProperty = AttributeSet))
	TArray<FGRAbilitySet_AttributeSet> GrantedAttributes;

private:
	void GrantAbilities(UGRAbilitySystemComponent* ASC, FGRAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
	void GrantGameplayEffects(UGRAbilitySystemComponent* ASC, FGRAbilitySet_GrantedHandles* OutGrantedHandles) const;
	void GrantAttributeSets(UGRAbilitySystemComponent* ASC, FGRAbilitySet_GrantedHandles* OutGrantedHandles) const;
};
