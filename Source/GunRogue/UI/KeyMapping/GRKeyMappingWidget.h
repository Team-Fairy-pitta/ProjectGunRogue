#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeyMappingWidget.generated.h"

class ULocalPlayer;
class UInputMappingContext;
class UScrollBox;
class UGRKeyMappingSlot;
class UEnhancedInputUserSettings;
struct FPlayerKeyMapping;

UCLASS()
class GUNROGUE_API UGRKeyMappingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGRKeyMappingSlot> SlotClass;

	void InitKeyMappings(TObjectPtr<UEnhancedInputUserSettings> UserSetting, TMap<FName, FPlayerKeyMapping>& Mappings);
	void ApplyKeyMappings(ULocalPlayer* InLocalPlayer, UInputMappingContext* MappingContext);
	void ClearWidgets();

	void StartChange(UGRKeyMappingSlot* Current);
	void ChangeKeyMapping(const FName& InActionName, const FKey& NewKey);
	void EndChange();
	bool IsChanging();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY()
	TObjectPtr<UGRKeyMappingSlot> CurrentChangingSlot = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnhancedInputUserSettings> CachedUserSetting;

	bool isChanging;
};
