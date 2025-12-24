#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "TestGA_BladeWaveFire.generated.h"

class UGRAbilitySystemComponent;

UCLASS()
class GRTEST_API UTestGA_BladeWaveFire : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTestGA_BladeWaveFire();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Tags")
	FGameplayTag Tag_BladeWaveMode;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_SizeAndDamageUp;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_SlowPierceAndDamageup;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_KillReduceSupportCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Projectile")
	TSubclassOf<AActor> BladeWaveProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Projectile")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Fire")
	float BaseFireInterval = 0.18f;

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Fire")
	float SlowFireIntervalMultiplier = 2.0f;
};
