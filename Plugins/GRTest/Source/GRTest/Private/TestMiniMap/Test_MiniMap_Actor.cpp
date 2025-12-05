#include "TestMiniMap/Test_MiniMap_Actor.h"

#include "AbilitySystemComponent.h"

ATest_MiniMap_Actor::ATest_MiniMap_Actor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ATest_MiniMap_Actor::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystem)
	{
		for (int32 i = 0; i < RadarTag.Num(); ++i)
		{
			AbilitySystem->AddLooseGameplayTag(RadarTag[i]);
		}
	}
	
}


