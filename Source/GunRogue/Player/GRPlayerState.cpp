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
#include "GameModes/Level1/GRGameMode_Level1.h"
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

	PreviousWeaponSlot = INDEX_NONE;
	CurrentWeaponSlot = INDEX_NONE;
	PredictedWeaponSlot = INDEX_NONE;
}

void AGRPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDead);

	DOREPLIFETIME(ThisClass, ItemHandles);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, CurrentWeaponSlot);

	DOREPLIFETIME(ThisClass, OwnedAugments);

	DOREPLIFETIME(ThisClass, CurrentMetaGoods);
	DOREPLIFETIME(ThisClass, Gold);
}

void AGRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
}

bool AGRPlayerState::IsDead() const
{
	return bIsDead == 1;
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
		ApplyAllPerksToASC();
		BindOnHealthChanged();

		OnRespawn();
	}
}

void AGRPlayerState::InitAbilitySystemComponent()
{
	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (bIsAbilitySystemComponentInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent Already Init..."));
		
		AbilitySystemComponent->SetAvatarActor(GRCharacter);
	}
	else
	{
		const UGRPawnData* PawnData = GRCharacter->GetPawnData();
		if (!PawnData)
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

		bIsAbilitySystemComponentInit = true;
	}
}

void AGRPlayerState::ApplyAllPerksToASC()
{
	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	if (GRCharacter->IsLocallyControlled())
	{
		InitPerkFromSave();
		ServerRPC_ApplyAllPerksToASC(PerkInfoRows);

		if (!HasAuthority())
		{
			ServerRPC_SetCurrentMetaGoods(CurrentMetaGoods);
		}
	}
}

void AGRPlayerState::BindOnHealthChanged()
{
	if (HasAuthority())
	{
		RemoveOnHealthChanged();
		AddOnHealthChanged();
	}
}

void AGRPlayerState::AddOnHealthChanged()
{
	OnHealthChangedHandle = 
		AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ThisClass::OnHealthChanged);
}

void AGRPlayerState::RemoveOnHealthChanged()
{
	if (OnHealthChangedHandle.IsValid())
	{
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetHealthAttribute())
			.Remove(OnHealthChangedHandle);

		OnHealthChangedHandle.Reset();
	}
}

void AGRPlayerState::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	if (Health <= 0)
	{
		OnDead();
	}
}

void AGRPlayerState::OnDead()
{
	if (HasAuthority() && !IsDead())
	{
		bIsDead = 1; // uint8 true

		AGRCharacter* GRCharacter = GetGRCharacter();
		if (!IsValid(GRCharacter))
		{
			return;
		}

		GRCharacter->MulticastRPC_OnDead();

		static const float BodyLifeSpan = 1.5f;
		GetWorldTimerManager().SetTimer(DeadTimer, this, &ThisClass::OnBodyExpired, BodyLifeSpan, false);
	}
}

void AGRPlayerState::OnRespawn()
{
	bIsDead = 0; // uint8 false

	if (SpectateTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SpectateTimer);
		SpectateTimer.Invalidate();
	}

	// HUD 정보 복구
	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	if (GRCharacter->IsLocallyControlled())
	{
		UpdateMetaGoodsUI();
		UpdateGoldUI();
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::ServerRPC_ResetWeaponHandles);
	}
}

void AGRPlayerState::RestoreHealthAndShield()
{
	// 체력과 생명력 복구
	if (!AbilitySystemComponent)
	{
		return;
	}

	float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UGRHealthAttributeSet::GetMaxHealthAttribute());
	float MaxSheild = AbilitySystemComponent->GetNumericAttribute(UGRHealthAttributeSet::GetMaxShieldAttribute());

	AbilitySystemComponent->SetNumericAttributeBase(UGRHealthAttributeSet::GetHealthAttribute(), MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UGRHealthAttributeSet::GetShieldAttribute(), MaxSheild);
}

void AGRPlayerState::OnBodyExpired()
{
	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	GRCharacter->Destroy();

	AGRPlayerController* GRPlayerController = GetGRPlayerController();
	if (!IsValid(GRPlayerController))
	{
		return;
	}

	AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(GRPlayerController);
	if (!IsValid(BattlePlayerController))
	{
		return;
	}

	bool bIsGameOver = false;
	if (GetWorld())
	{
		AGRGameMode_Level1* GameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
		if (IsValid(GameMode))
		{
			bIsGameOver = GameMode->CheckGameOver();
		}
	}

	if (!bIsGameOver)
	{
		BattlePlayerController->ClientRPC_StartSpectating();

		// [NOTE] GRCharacter->Destroy(); 이후에 ServerRPC_StartSpectating()가 호출 되어야 함
		static const float WaitForActorDestroy = 0.1f;
		GetWorldTimerManager().SetTimer(
			SpectateTimer,
			BattlePlayerController,
			&AGRBattlePlayerController::ServerRPC_StartSpectating,
			WaitForActorDestroy,
			false);
	}
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


