#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeyMappingWidget.generated.h"

class UScrollBox;
class UGRKeyMappingSlot;
struct FPlayerKeyMapping;

UCLASS()
class GUNROGUE_API UGRKeyMappingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGRKeyMappingSlot> SlotClass;

	void InitKeyMappings(TMap<FName, FPlayerKeyMapping>& Mappings);
	void ClearWidgets();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;
};
