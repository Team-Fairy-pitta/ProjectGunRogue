// GRBattleHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRBattleHUDWidget.generated.h"

class UGRMiniMapWidget;
class UGRLevelStatusWidget;
class UGRNotifyMessageWidget;
class UGRGoodsTextWidget;
class UGRCrosshairWidget;
class UGRPlayerStatusWidget;
class UGRTeamStatusListWidget;
class UGRWeaponListWidget;
class UGRSkillListWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBattleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRLevelStatusWidget* GetLevelStatusWidget() const { return LevelStatus; }

	UGRNotifyMessageWidget* GetNotifyMessageWidget() const { return NotifyMessage; }

	UGRGoodsTextWidget* GetGoodsTextWidget() const { return GoodsText; }

	UGRTeamStatusListWidget* GetTeamStatusListWidget() const { return TeamStatusList; }
	
	UGRPlayerStatusWidget* GetPlayerStatusWidget() const { return PlayerStatus; }

	UGRSkillListWidget* GetSkillListWidget() const { return SkillList; }

	UGRWeaponListWidget* GetWeaponListWidget() const { return WeaponList; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRMiniMapWidget* Minimap;
	
	UPROPERTY(meta = (BindWidget))
	UGRLevelStatusWidget* LevelStatus;

	UPROPERTY(meta = (BindWidget))
	UGRNotifyMessageWidget* NotifyMessage;

	UPROPERTY(meta = (BindWidget))
	UGRGoodsTextWidget* GoodsText;

	UPROPERTY(meta = (BindWidget))
	UGRCrosshairWidget* Crosshair;

	UPROPERTY(meta = (BindWidget))
	UGRPlayerStatusWidget* PlayerStatus;

	UPROPERTY(meta = (BindWidget))
	UGRTeamStatusListWidget* TeamStatusList;

	UPROPERTY(meta = (BindWidget))
	UGRWeaponListWidget* WeaponList;

	UPROPERTY(meta = (BindWidget))
	UGRSkillListWidget* SkillList;
};
