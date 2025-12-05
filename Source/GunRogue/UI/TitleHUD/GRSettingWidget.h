#pragma once

#include "Blueprint/UserWidget.h"
#include "GRSettingWidget.generated.h"

class UButton;
class UWidgetSwitcher;
class UGRKeySettingWidget;

UCLASS()
class GUNROGUE_API UGRSettingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GunRogue|Config")
	FLinearColor TabActiveBackgroundColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GunRogue|Config")
	FLinearColor TabInactiveBackgroundColor;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TabButton_Game;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TabButton_Key;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TabButton_Other;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> SettingTab;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGRKeySettingWidget> KeySettingWidget;

private:
	void SetTabButtonColor(int32 Index);

	UFUNCTION()
	void OnClicked_BackToMenuButton();

	UFUNCTION()
	void OnClicked_TabButton_Game();

	UFUNCTION()
	void OnClicked_TabButton_Key();

	UFUNCTION()
	void OnClicked_TabButton_Other();
};
