#pragma once

#include "Blueprint/UserWidget.h"
#include "GRLevel1RoomWidget.generated.h"

struct FGRLevel1Node;
class UBorder;
class UTextBlock;
class UButton;
class UGRLevel1SelectWidget;

UCLASS()
class GUNROGUE_API UGRLevel1RoomWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitRoomWidget(int32 InIndex, const FGRLevel1Node& Level1Data, UGRLevel1SelectWidget* InParentWidget);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RoomButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MapIndex;

	UFUNCTION()
	void OnButtonClicked();

	UGRLevel1SelectWidget* ParentWidget;
	int32 Index = -1;
};
