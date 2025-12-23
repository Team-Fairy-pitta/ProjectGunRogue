// GRTeamStatusListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTeamStatusListWidget.generated.h"

class UGRTeamStatusWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTeamStatusListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* TeamStatusContainer;
	
	UPROPERTY(EditDefaultsOnly, Category="TeamStatus")
	TSubclassOf<UGRTeamStatusWidget> TeamStatusClass;

	UPROPERTY()
	TArray<UGRTeamStatusWidget*> TeamStatusArray;

public:
	void SetTeamShieldBar(int32 Index, float CurrentShield, float MaxShield);

	void SetTeamHPBar(int32 Index, float CurrentHP, float MaxHP);

	void SetTeamCharacterThumbnail(int32 Index, UTexture2D* Thumbnail);

	UFUNCTION(BlueprintCallable)
	void CreateTeamStatus();

	void DestroyLastTeamStatus();

	int32 GetTeamStatusWidgetCount() const;
};
