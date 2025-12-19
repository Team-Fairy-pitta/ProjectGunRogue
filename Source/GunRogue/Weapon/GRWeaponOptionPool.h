#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponOptionPool.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGRValueApplyType : uint8
{	
	Plus,		// +
	Minus,		// -		
	Multiply,	// *
	Percent,	// %

};

UENUM(BlueprintType)
enum class EGRConditionType : uint8
{
	Ammo,
	HP,
};

USTRUCT(BlueprintType)
struct FGRConditionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGRConditionType ConditionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGRValueApplyType ApplyType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue = 0.f;
};


UENUM(BlueprintType)
enum class EGREffectType : uint8
{
	Damage,
	FireRate,
	ReloadSpeed,
};

USTRUCT(BlueprintType)
struct FGREffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGREffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGRValueApplyType ApplyType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue = 0.f;
};

USTRUCT(BlueprintType)
struct FOptionPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGRGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere)
	TArray<FGRConditionData> Conditions;

	UPROPERTY(EditAnywhere)
	TArray<FGREffectData> Effects;

};

UCLASS()
class GUNROGUE_API UGRWeaponOptionPool : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FOptionPoolEntry> Options;
};
