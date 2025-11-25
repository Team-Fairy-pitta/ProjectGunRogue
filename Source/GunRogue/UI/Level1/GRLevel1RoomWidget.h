#pragma once

#include "Blueprint/UserWidget.h"
#include "GRLevel1RoomWidget.generated.h"

struct FGRLevel1Node;
class UBorder;
class UTextBlock;

UCLASS()
class GUNROGUE_API UGRLevel1RoomWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitRoomWidget(const FGRLevel1Node& Level1Data);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MapIndex;
};
