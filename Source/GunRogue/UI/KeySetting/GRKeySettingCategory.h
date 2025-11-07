#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingCategory.generated.h"

class UVerticalBox;
class UTextBlock;

UCLASS()
class GUNROGUE_API UGRKeySettingCategory : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MappingsVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CategoryText;
};
