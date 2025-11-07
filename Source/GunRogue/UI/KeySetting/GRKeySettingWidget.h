#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingWidget.generated.h"

class UEnhancedInputUserSettings;

UCLASS()
class GUNROGUE_API UGRKeySettingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitKeyMappings();


private:
	void SetupUserSetting();

	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> CachedUserSetting;
};
