#include "UI/MiniMap/GRRadarMapWidget.h"

#include "UI/MiniMap/GRRadarIconWidget.h"
#include "Components/CanvasPanel.h"

void UGRRadarMapWidget::UpdateRadar(const TArray<FRadarTargetInfo>& Targets)
{
	TSet<AActor*> CurrentTargets;

	for (const FRadarTargetInfo& Info : Targets)
	{
		if (!IsValid(Info.TargetActor))
		{
			continue;
		}

		CurrentTargets.Add(Info.TargetActor);

		UGRRadarIconWidget* Icon = nullptr;

		if (!IconMap.Contains(Info.TargetActor))
		{
			Icon = CreateWidget<UGRRadarIconWidget>(this, IconWidgetClass);
			IconCanvas->AddChild(Icon);

			Icon->InitIcon(Info.RadarTag);

			IconMap.Add(Info.TargetActor, Icon);
		}
		else
		{
			Icon = IconMap[Info.TargetActor];
		}

		Icon->UpdateRadarPosition(Info.NormalizedTargetDirection, GetRadarSize());
	}

	// 제거할 아이콘 찾기
	TArray<AActor*> RemoveList;

	for (auto& Pair : IconMap)
	{
		if (!CurrentTargets.Contains(Pair.Key))
		{
			RemoveList.Add(Pair.Key);
		}
	}

	// 아이콘 제거
	for (AActor* Actor : RemoveList)
	{
		if (IconMap.Contains(Actor))
		{
			IconMap[Actor]->RemoveFromParent();
			IconMap.Remove(Actor);
		}
	}
}

float UGRRadarMapWidget::GetRadarSize() const 
{
	if (!IconCanvas)
	{
		return 0.0f;
	}

	FVector2D Size = IconCanvas->GetCachedGeometry().GetLocalSize();

	return FMath::Min(Size.X, Size.Y);
}
