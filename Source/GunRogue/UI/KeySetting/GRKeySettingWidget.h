#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingWidget.generated.h"

class UEnhancedInputUserSettings;
class UVerticalBox;
class UButton;
class UGRKeySettingCategory;
struct FPlayerKeyMapping;

struct FKeyMappingCategory
{
	FText CategoryText;
	TArray<FPlayerKeyMapping> Mappings;
};

UCLASS()
class GUNROGUE_API UGRKeySettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitKeyMappings();

	void UpdateMappings(const FName& InMappingName, const FKey& NewKey);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGRKeySettingCategory> CategoryWidgetClass;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoriesVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetButton;

private:
	UEnhancedInputUserSettings* LoadUserSetting();
	void GetKeyMappings(TMap<FString, FKeyMappingCategory>& OutMappings);

	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> CachedUserSetting;
};
