#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GRRadarIconWidget.generated.h"

class UImage;
class UGRRadarMapIconDataAsset;

UCLASS()
class GUNROGUE_API UGRRadarIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	

	void InitIcon(const FGameplayTag& InTag);
	void UpdateRadarPosition(const FVector2D& ActorPos, const float RadarSize);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Radar")
	UGRRadarMapIconDataAsset* IconDataAsset;
};
