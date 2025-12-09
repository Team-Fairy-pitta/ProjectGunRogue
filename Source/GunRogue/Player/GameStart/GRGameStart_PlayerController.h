#pragma once

#include "Player/GRPlayerController.h"
#include "GRGameStart_PlayerController.generated.h"

class UGRTitleHUDWidget;
class UGRSettingWidget;

UCLASS()
class GUNROGUE_API AGRGameStart_PlayerController : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	AGRGameStart_PlayerController();
	virtual void BeginPlay() override;

	void ShowTitleWidget();

	void ShowSettingWidget();
	void HideSettingWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRTitleHUDWidget> TitleWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRTitleHUDWidget> TitleWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRSettingWidget> SettingWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRSettingWidget> SettingWidgetInstance;

private:
	void CreateWidgets();
};
