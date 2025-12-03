#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMap/GRRadarMapComponent.h"
#include "GRRadarMapWidget.generated.h"

class UGRRadarIconWidget;

UCLASS()
class GUNROGUE_API UGRRadarMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateRadar(const TArray<FRadarTargetInfo>& Targets);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar UI")
	TSubclassOf<UGRRadarIconWidget> IconWidgetClass;

	UPROPERTY()
	TMap<AActor*, UGRRadarIconWidget*> IconMap;
};
