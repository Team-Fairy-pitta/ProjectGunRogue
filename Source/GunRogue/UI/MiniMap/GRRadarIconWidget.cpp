#include "UI/MiniMap/GRRadarIconWidget.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UGRRadarIconWidget::InitIcon(const FGameplayTag& InTag)
{
	RadarTag = InTag;

	if (InTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Minimap.Type.Player")))
	{
		IconImage->SetBrushFromTexture(PlayerTexture);
	}
	else if (InTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Minimap.Type.Enemy")))
	{
		IconImage->SetBrushFromTexture(EnemyTexture);
	}
	else if (InTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Minimap.Type.Item")))
	{
		IconImage->SetBrushFromTexture(ItemTexture);
	}
}

void UGRRadarIconWidget::UpdateRadarPosition(const FVector2D& RadarPos)
{
	//FVector2D MapCenter(960, 540); // 또는 부모 위젯의 중심 계산
	//FVector2D Pixel = MapCenter + RadarPos * RadarScale * 1000.f;

	//SetRenderTranslation(Pixel);

	SetRenderTranslation(RadarPos * 200.f);
}
