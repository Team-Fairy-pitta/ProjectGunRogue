#pragma once

#include "GameplayEffect.h"
#include "GRGameplayEffect.generated.h"

class UTexture;

UCLASS()
class GUNROGUE_API UGRGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Icon")
	TObjectPtr<UTexture> EffectIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Name")
	FText EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Description")
	FText EffectDescription;
};
