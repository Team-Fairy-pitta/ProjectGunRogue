// GRTeamStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTeamStatusWidget.generated.h"

class UGRHPBarWidget;
class UImage;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTeamStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRHPBarWidget* TeamHPBar;

	UPROPERTY(meta = (BindWidget))
	UImage* TeamFace;

	virtual void NativeConstruct() override;

public:
	void SetTeamShieldBar(float CurrentShield, float MaxShield);
	
	void SetTeamHPBar(float CurrentHP, float MaxHP);

	void SetTeamCharacterThumbnail(UTexture2D* Thumbnail);
};
