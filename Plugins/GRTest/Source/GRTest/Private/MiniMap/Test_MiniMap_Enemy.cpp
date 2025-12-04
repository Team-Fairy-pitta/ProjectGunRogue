#include "MiniMap/Test_MiniMap_Enemy.h"

#include "AbilitySystemComponent.h"

ATest_MiniMap_Enemy::ATest_MiniMap_Enemy()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ATest_MiniMap_Enemy::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystem)
	{
		FGameplayTag MiniMapShowTag = FGameplayTag::RequestGameplayTag(FName("MiniMap.Show"));
		AbilitySystem->AddLooseGameplayTag(MiniMapShowTag);

		FGameplayTag MiniMapenemyTag = FGameplayTag::RequestGameplayTag(FName("MiniMap.Type.Enemy"));
		AbilitySystem->AddLooseGameplayTag(MiniMapenemyTag);
	}
	
}


