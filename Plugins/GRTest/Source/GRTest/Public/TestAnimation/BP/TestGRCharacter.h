#pragma once

#include "CoreMinimal.h"
#include "Character/GRCharacter.h"
#include "AbilitySystemInterface.h"
#include "TestGRCharacter.generated.h"

class UAbilitySystemComponent;
class UTestWeaponAsset;

UCLASS()
class GRTEST_API ATestGRCharacter : public AGRCharacter
{
	GENERATED_BODY()
	
public:
	ATestGRCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UTestWeaponAsset> CurrentWeaponAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bHasWeapon = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void PushWeaponStateToAnimBP();
};
