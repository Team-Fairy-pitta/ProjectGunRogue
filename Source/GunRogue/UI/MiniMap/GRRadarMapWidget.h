#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMap/GRRadarMapComponent.h"
#include "GRRadarMapWidget.generated.h"

class UGRRadarIconWidget;
class UCanvasPanel;

UCLASS()
class GUNROGUE_API UGRRadarMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateRadar(const TArray<FRadarTargetInfo>& Targets);

	float GetRadarSize() const;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar UI")
	TSubclassOf<UGRRadarIconWidget> IconWidgetClass;

	UPROPERTY()
	TMap<AActor*, UGRRadarIconWidget*> IconMap;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* IconCanvas;
};
