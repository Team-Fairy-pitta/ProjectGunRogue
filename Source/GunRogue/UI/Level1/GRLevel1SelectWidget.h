#pragma once

#include "Blueprint/UserWidget.h"
#include "GRLevel1SelectWidget.generated.h"

class UUniformGridPanel;
class UGRLevel1RoomWidget;
class AGRLevel1ControlPanel;
struct FGRLevel1Data;

UCLASS()
class GUNROGUE_API UGRLevel1SelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void InitWidget(const FGRLevel1Data& Level1Data, AGRLevel1ControlPanel* ControlPanel);
	void ResetWidget();

	void OnRoomClicked(int32 Index);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> RoomGridPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue")
	TSubclassOf<UGRLevel1RoomWidget> RoomWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UGRLevel1RoomWidget>> RoomWidgetInstances;

private:
	void CreateRoomWidgets();
	void SetWidgetFocusable();

	AGRLevel1ControlPanel* CachedControlPanel;
};
