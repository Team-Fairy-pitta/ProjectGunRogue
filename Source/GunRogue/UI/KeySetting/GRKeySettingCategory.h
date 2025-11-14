#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingCategory.generated.h"

class UVerticalBox;
class UTextBlock;
class UGRKeySettingSlot;
class UGRKeySettingWidget;
struct FPlayerKeyMapping;

UCLASS()
class GUNROGUE_API UGRKeySettingCategory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetCategoryText(const FText& InText);
	void AddMappings(const TArray<FPlayerKeyMapping>& InMappings, UGRKeySettingWidget* InParentWidget);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGRKeySettingSlot> SlotWidgetClass;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MappingsVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CategoryText;
};
