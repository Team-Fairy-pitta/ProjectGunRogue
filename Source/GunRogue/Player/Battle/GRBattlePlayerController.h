#pragma once

#include "Player/GRPlayerController.h"
#include "GRBattlePlayerController.generated.h"

class UGRBattleHUDWidget;

UCLASS()
class GUNROGUE_API AGRBattlePlayerController : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	AGRBattlePlayerController();
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY()
	TObjectPtr<UGRBattleHUDWidget> HUDWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRBattleHUDWidget> HUDWidgetClass;

	void ShowBattleHUD();

private:
	void CreateWidgets();
	void InitBattleHUD();
};
