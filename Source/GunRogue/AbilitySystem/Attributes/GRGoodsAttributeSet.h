// GRGoodsAttributeSet.h

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRGoodsAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRGoodsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGRGoodsAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_GoldMultiplier, Category = "Gold|Perk")
	FGameplayAttributeData GoldMultiplier;
	ATTRIBUTE_ACCESSORS(UGRGoodsAttributeSet, GoldMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_GemMultiplier, Category = "Gem|Perk")
	FGameplayAttributeData GemMultiplier;
	ATTRIBUTE_ACCESSORS(UGRGoodsAttributeSet, GemMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Gold|Meta")
	FGameplayAttributeData GainGold;
	ATTRIBUTE_ACCESSORS(UGRGoodsAttributeSet, GainGold);

	UPROPERTY(BlueprintReadOnly, Category = "Gem|Meta")
	FGameplayAttributeData GainGem;
	ATTRIBUTE_ACCESSORS(UGRGoodsAttributeSet, GainGem);

protected:
	UFUNCTION()
	void OnRep_GoldMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_GemMultiplier(const FGameplayAttributeData& OldValue);
};
