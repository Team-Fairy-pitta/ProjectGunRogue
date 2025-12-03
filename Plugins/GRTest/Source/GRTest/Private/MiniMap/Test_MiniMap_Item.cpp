#include "MiniMap/Test_MiniMap_Item.h"

#include "AbilitySystemComponent.h"

ATest_MiniMap_Item::ATest_MiniMap_Item()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ATest_MiniMap_Item::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystem)
	{
		FGameplayTag MiniMapShowTag = FGameplayTag::RequestGameplayTag(FName("MiniMap.Show"));
		AbilitySystem->AddLooseGameplayTag(MiniMapShowTag);

		FGameplayTag MiniMapItemTag = FGameplayTag::RequestGameplayTag(FName("MiniMap.Type.Item"));
		AbilitySystem->AddLooseGameplayTag(MiniMapItemTag);
	}
	
}


