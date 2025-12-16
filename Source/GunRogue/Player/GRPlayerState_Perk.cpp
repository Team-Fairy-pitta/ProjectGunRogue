#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Character/GRCharacter.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "MetaProgression/GRPerkStructs.h"
#include "MetaProgression/PerkInfoRow.h"


FString GetPIENetModeString(const UWorld* World);

#pragma region Perk;
void AGRPlayerState::InitPerkInfoRows()
{
	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return;
	}

	UDataTable* PerkTable = PerkSubsystem->GetPerkTable();
	if (!PerkTable)
	{
		return;
	}

	PerkInfoRows.Empty();

	TArray<FPerkInfoRow*> Rows;
	PerkTable->GetAllRows(TEXT(""), Rows);

	for (FPerkInfoRow* Row : Rows)
	{
		FPerkEntry NewEntry;
		NewEntry.PerkID = Row->PerkID;
		NewEntry.Level = 0;
		PerkInfoRows.Add(NewEntry);
	}
}

void AGRPlayerState::InitPerkFromSave()
{
	InitPlayerID();

	if (PlayerID.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState: PlayerID not ready yet"));
		return;
	}

	InitPerkInfoRows();

	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState: PerkSubsystem not"));
		return;
	}

	TArray<FPerkEntry> TempPerkInfoRows;
	int32 TempMetaGoods;

	if (PerkSubsystem->LoadPerks(PlayerID, TempPerkInfoRows, TempMetaGoods))
	{
		LoadPerkFromSave(TempPerkInfoRows, TempMetaGoods);
	}
}

void AGRPlayerState::LoadPerkFromSave(const TArray<FPerkEntry>& LoadedPerkInfoRows, int32 LoadedMetaGoods)
{
	CurrentMetaGoods = LoadedMetaGoods;

	for (const FPerkEntry& SavedEntry : LoadedPerkInfoRows)
	{
		FName SavedPerkID = SavedEntry.PerkID;
		int32 SavedLevel = SavedEntry.Level;

		for (FPerkEntry& Entry : PerkInfoRows)
		{
			if (Entry.PerkID == SavedPerkID)
			{
				Entry.Level = SavedLevel;
				break;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("=== LoadPerkFromSave ==="));
	UE_LOG(LogTemp, Warning, TEXT("CurrentMetaGoods = %d"), CurrentMetaGoods);
	UE_LOG(LogTemp, Warning, TEXT("PerkInfoRows Count = %d"), PerkInfoRows.Num());

	for (const FPerkEntry& Entry : PerkInfoRows)
	{
		UE_LOG(LogTemp, Warning,
			TEXT(" PerkID = %s, Level = %d"),
			*Entry.PerkID.ToString(), Entry.Level);
	}
}

void AGRPlayerState::SavePerkToSave()
{
	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return;
	}

	PerkSubsystem->SavePerks(PlayerID, PerkInfoRows, CurrentMetaGoods);
}

void AGRPlayerState::InitPlayerID()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->IsPlayInEditor())
	{
#if WITH_EDITOR
		if (GetWorld())
		{
			PlayerID = GetPIENetModeString(GetWorld());
		}
#endif
	}
	else
	{
		// [NOTE] 실제 환경에서는 PlayerID를 구분할 필요가 없음 (게임이 하나만 실행되므로)
		PlayerID = FString::Printf(TEXT("LocalPlayer"));
	}
}

void AGRPlayerState::ServerRPC_SetCurrentMetaGoods_Implementation(int32 InMetaGoods)
{
	CurrentMetaGoods = InMetaGoods;	
}

void AGRPlayerState::ServerRPC_ApplyAllPerksToASC_Implementation(const TArray<FPerkEntry>& PerkInfos)
{
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	if (!ASC || !PerkGE)
	{
		return;
	}

	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return;
	}

	UDataTable* PerkTable = PerkSubsystem->GetPerkTable();
	if (!PerkTable)
	{
		return;
	}

	FGameplayTag PerkRootTag = FGameplayTag::RequestGameplayTag(FName("Perk"));
	ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(PerkRootTag));

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(PerkGE, 1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->DynamicGrantedTags.AddTag(PerkRootTag);

	UE_LOG(LogTemp, Warning, TEXT("PerkInfos.Num() = %d"), PerkInfos.Num());

	for (const FPerkEntry& Entry : PerkInfos)
	{
		FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(Entry.PerkID, TEXT(""));
		if (!Row)
			continue;

		float LevelBonus = Row->ValuePerLevel * Entry.Level;
		float FinalValue = LevelBonus;

		if (Entry.PerkID == FName("Health") || Entry.PerkID == FName("Shield") || Entry.PerkID == FName("ShieldInvincibleTime") ||
			Entry.PerkID == FName("ShotgunTraining") || Entry.PerkID == FName("SniperTraining") || Entry.PerkID == FName("WeaponDamage"))
		{
			FinalValue = LevelBonus;
		}
		else if (Entry.PerkID == FName("ShieldCooldown") || Entry.PerkID == FName("StrengthenShield") || Entry.PerkID == FName("PistolTraining"))
		{
			FinalValue = FMath::Clamp(1.0f - LevelBonus, 0.0f, 1.0f);
		}
		else if (Entry.PerkID == FName("RifleTraining"))
		{
			FinalValue = FMath::Clamp(1.0f + LevelBonus, 0.0f, 10.0f);
		}
		else
		{
			continue;
		}

		SpecHandle.Data->SetSetByCallerMagnitude(Row->PerkTag, FinalValue);
		UE_LOG(LogTemp, Warning, TEXT(" -> SetByCaller %s = %f"), *Row->PerkTag.ToString(), FinalValue);
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
#pragma endregion

// AI의 도움을 받아 생성한 PIE용 함수
FString GetPIENetModeString(const UWorld* World)
{
#if WITH_EDITOR
	if (!World || !GEngine) return TEXT("");

	auto& Contexts = GEngine->GetWorldContexts();

	for (const FWorldContext& Ctx : Contexts)
	{
		if (Ctx.World() == World && Ctx.WorldType == EWorldType::PIE)
		{
			const int32 Instance = Ctx.PIEInstance;

			// Server or Client 판별
			const ENetMode NetMode = World->GetNetMode();

			if (NetMode == NM_DedicatedServer)
			{
				return FString::Printf(TEXT("Dedicated Server %d"), Instance);
			}
			else if (NetMode == NM_ListenServer)
			{
				return FString::Printf(TEXT("Listen Server %d"), Instance);
			}
			else if (NetMode == NM_Client)
			{
				return FString::Printf(TEXT("Client %d"), Instance);
			}
			else
			{
				return FString::Printf(TEXT("Standalone %d"), Instance);
			}
		}
	}
#endif
	return TEXT("");
}
