#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GRInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputMappintContextAndPriority
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FInputActionWithTag
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


UCLASS(BlueprintType, Const)
class GUNROGUE_API UGRInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UGRInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionByTag(const FGameplayTag& InputTag) const;

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionByTag(const FGameplayTag& InputTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Mapping Context")
	TArray<FInputMappintContextAndPriority> InputMappings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Action with Tag")
	TArray<FInputActionWithTag> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Action with Tag")
	TArray<FInputActionWithTag> AbilityInputActions;
};
