#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TestWeaponAsset.generated.h"

class UAnimMontage;
class UAnimSequenceBase;
class USkeletalMesh;

UCLASS()
class GRTEST_API UTestWeaponAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FGameplayTagContainer WeaponTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USkeletalMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	UAnimSequenceBase* UpperBodyIdle;
};
