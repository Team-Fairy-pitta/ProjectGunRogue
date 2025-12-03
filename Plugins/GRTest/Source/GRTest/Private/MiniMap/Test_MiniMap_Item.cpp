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

	AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("MiniMap.Show"));

	AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("MiniMap.Item"));
	
}


