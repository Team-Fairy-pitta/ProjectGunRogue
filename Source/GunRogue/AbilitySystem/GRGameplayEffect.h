#pragma once

#include "GameplayEffect.h"
#include "GRGameplayEffect.generated.h"

class UTexture2D;

UCLASS()
class GUNROGUE_API UGRGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Icon")
	TObjectPtr<UTexture2D> EffectIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Name")
	FText EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Description")
	FText EffectDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Configuration")
	bool bShowBuffIconInUI = false;
};
