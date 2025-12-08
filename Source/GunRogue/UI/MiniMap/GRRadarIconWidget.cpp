#include "UI/MiniMap/GRRadarIconWidget.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MiniMap/GRRadarMapIconDataAsset.h"

void UGRRadarIconWidget::InitIcon(const FGameplayTag& InTag)
{

	if (!IconDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("IconDataAsset is not assigned!"));
		return;
	}

	const FRadarIconData* Data = IconDataAsset->FindIconData(InTag);
	if (!Data)
	{
		UE_LOG(LogTemp, Warning, TEXT("No icon found for Tag: %s"), *InTag.ToString());
		return;
	}

	IconImage->SetBrushFromTexture(Data->IconTexture);
	IconImage->SetColorAndOpacity(Data->IconColor);
	IconImage->SetRenderScale(Data->IconScale);
}

void UGRRadarIconWidget::UpdateRadarPosition(const FVector2D& ActorPos, const float RadarSize)
{
	FVector2D RadarPosition(ActorPos.Y, -ActorPos.X);

	RadarPosition *= RadarSize;
	RadarPosition.X += RadarSize / 2.0f;
	RadarPosition.Y += RadarSize / 2.0f;

	SetRenderTranslation(RadarPosition);
}
