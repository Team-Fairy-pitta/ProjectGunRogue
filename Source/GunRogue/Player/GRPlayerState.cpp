#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "Net/UnrealNetwork.h"


AGRPlayerState::AGRPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystem 네트워크 관련: needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetted);

	// 무기 슬롯 초기화 (2개)
	WeaponSlots.SetNum(WeaponSlot::MaxWeaponSlots);
}

void AGRPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AGRPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemHandles);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, CurrentWeaponSlot);

	DOREPLIFETIME(ThisClass, OwnedAugments);

	DOREPLIFETIME(ThisClass, CurrentMetaGoods);
}

void AGRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
}

AGRPlayerController* AGRPlayerState::GetGRPlayerController() const
{
	return Cast<AGRPlayerController>(GetOwner());
}

AGRCharacter* AGRPlayerState::GetGRCharacter() const
{
	return Cast<AGRCharacter>(GetPawn());
}

UGRAbilitySystemComponent* AGRPlayerState::GetGRAbilitySystemComponent()
{
	return Cast<UGRAbilitySystemComponent>(AbilitySystemComponent);
}

UAbilitySystemComponent* AGRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGRPlayerState::OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (IsValid(NewPawn))
	{
		InitAbilitySystemComponent();
	}
}

void AGRPlayerState::InitAbilitySystemComponent()
{
	if (bIsAbilitySystemComponentInit)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent Already Init..."));
		return;
	}

	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	const UGRPawnData* PawnData = GRCharacter->GetPawnData();
	if (!PawnData)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this /*Owner*/, GRCharacter /*Avatar*/);

	for (UGRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}

	if (OnAbilitySystemComponentInit.IsBound())
	{
		OnAbilitySystemComponentInit.Broadcast();
	}

	if (GRCharacter->IsLocallyControlled())
	{
		InitPerkFromSave();
		ServerRPC_ApplyAllPerksToASC(PerkInfoRows);
	}

	bIsAbilitySystemComponentInit = true;
}

FVector AGRPlayerState::GetGroundPointUsingLineTrace(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return FVector::ZeroVector;
	}
	if (!GetWorld())
	{
		return SpawnedActor->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;
	FVector Start = SpawnedActor->GetActorLocation();
	FVector Result = Start;
	FVector End = Start + FallDirection * (CheckDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SpawnedActor);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
}

void AGRPlayerState::PlaceActorOnGround(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return;
	}

	FVector BoxOrigin;
	FVector BoxExtent;
	SpawnedActor->GetActorBounds(true, BoxOrigin, BoxExtent);
	FVector NewLocation = GetGroundPointUsingLineTrace(SpawnedActor);
	NewLocation.Z += BoxExtent.Z;
	SpawnedActor->SetActorLocation(NewLocation);
}


