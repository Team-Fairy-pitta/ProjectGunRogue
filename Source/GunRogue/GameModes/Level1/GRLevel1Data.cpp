#include "GameModes/Level1/GRLevel1Data.h"


void FGRLevel1Data::Init()
{
	bIsValid = true;
}

void FGRLevel1Data::PrintDebugLog()
{
	if (IsValid())
	{

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FGRLevel1Data is INVALID"));
	}
}
