#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeyMappingWidget.generated.h"

class UScrollBox;
class UGRKeyMappingSlot;

UCLASS()
class GUNROGUE_API UGRKeyMappingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGRKeyMappingSlot> SlotClass;


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;
};
