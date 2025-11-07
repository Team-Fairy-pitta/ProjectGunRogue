#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingWidget.generated.h"

class UEnhancedInputUserSettings;
class UVerticalBox;
class UButton;

UCLASS()
class GUNROGUE_API UGRKeySettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitKeyMappings();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoriesVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetButton;

private:
	void SetupUserSetting();

	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> CachedUserSetting;
};
