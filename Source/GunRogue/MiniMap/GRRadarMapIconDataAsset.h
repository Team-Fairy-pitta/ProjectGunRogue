#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GRRadarMapIconDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FRadarIconData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Categories = "MiniMap"))
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* IconTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor IconColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D IconScale = FVector2D(1.f, 1.f);
};

UCLASS()
class GUNROGUE_API UGRRadarMapIconDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRadarIconData> IconDataList;

	const FRadarIconData* FindIconData(const FGameplayTag& Tag) const;
};
