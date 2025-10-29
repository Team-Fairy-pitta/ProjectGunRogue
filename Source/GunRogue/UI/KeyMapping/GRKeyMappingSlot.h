#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeyMappingSlot.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class GUNROGUE_API UGRKeyMappingSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FName& ActionName, const FName& KeyName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MappingChangeButton;
};
