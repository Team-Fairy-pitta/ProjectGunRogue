#pragma once

#include "Blueprint/UserWidget.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRLevel1RoomWidget.generated.h"

struct FGRLevel1Node;
class UBorder;
class UTextBlock;
class UButton;
class UImage;
class UTexture2D;
class UGRLevel1SelectWidget;

UCLASS()
class GUNROGUE_API UGRLevel1RoomWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void InitRoomWidget(int32 InIndex, const FGRLevel1Node& Level1Data, UGRLevel1SelectWidget* InParentWidget);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UTexture2D> BaseIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UTexture2D> HardIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UTexture2D> BossIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FSlateColor BaseIconColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FSlateColor HardIconColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	FSlateColor BossIconColor;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RoomButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TypeIcon;

	UFUNCTION()
	void OnButtonClicked();

	UGRLevel1SelectWidget* ParentWidget;
	FGRLevel1Node CachedNodeInfo;
	int32 Index = -1;
};
