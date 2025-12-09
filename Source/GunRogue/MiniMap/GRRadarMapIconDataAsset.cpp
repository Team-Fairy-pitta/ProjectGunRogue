#include "MiniMap/GRRadarMapIconDataAsset.h"

const FRadarIconData* UGRRadarMapIconDataAsset::FindIconData(const FGameplayTag& Tag) const
{
	for (const FRadarIconData& Data : IconDataList)
	{
		if (Tag.MatchesTag(Data.Tag))
		{
			return &Data;
		}
	}

	return nullptr;
}
