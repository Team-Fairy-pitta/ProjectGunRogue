#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"

AGRBattlePlayerController::AGRBattlePlayerController()
{
}

void AGRBattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Standalone 모드에서 테스트하는 경우, OnRep_PlayerState()을 직접 호출해 주어야 한다.
	// 또한, Editor가 아닌 곳에서 Standalone으로 실행되어서는 안된다.
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
#if WITH_EDITOR
		OnRep_PlayerState();
#else
		UE_LOG(LogTemp, Fatal, TEXT("BattlePlayerController: requires ListenServer or Client!!"));
#endif
	}
	// ListenServer 모드로 실행된 서버의 경우에도, OnRep_PlayerState()을 직접 호출해 주어야 한다.
	// 서버에서는 PlayerController가 먼저 Spawn되고, 그 이후 PlayerState가 Spawn되므로 문제가 없다.
	else if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		if (IsLocalController())
		{
			OnRep_PlayerState();
		}
	}
}

void AGRBattlePlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (OtherPlayerStatusUpdateTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(OtherPlayerStatusUpdateTimer);
	}
}

void AGRBattlePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트에 PlayerController와 PlayerState가 모두 준비되어야 HUD를 초기화할 수 있다.

	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	
	}
	
	if (GRPlayerState->IsAbilitySystemComponentInit())
	{
		InitUISetup();
	}
	else
	{
		GRPlayerState->OnAbilitySystemComponentInit.AddUObject(this, &ThisClass::InitUISetup);
	}
}

void AGRBattlePlayerController::InitUISetup()
{
	CreateWidgets();
	InitBattleHUD();
	ShowBattleHUD();
}

void AGRBattlePlayerController::ShowBattleHUD()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance is INVALID"));
		return;
	}
	if (!HUDWidgetInstance->IsInViewport())
	{
		HUDWidgetInstance->AddToViewport();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRBattlePlayerController::CreateWidgets()
{
	if(!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass (TSubclassOf<UGRBattleHUDWidget>) is INVALID"));
		return;
	}

	HUDWidgetInstance = CreateWidget<UGRBattleHUDWidget>(this, HUDWidgetClass);
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRBattleHUDWidget Widgets"));
		return;
	}
}

void AGRBattlePlayerController::InitBattleHUD()
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}

	const UAttributeSet* AttributeSet = ASC->GetAttributeSet(UGRHealthAttributeSet::StaticClass());
	const UGRHealthAttributeSet* HealthSet = Cast<UGRHealthAttributeSet>(AttributeSet);
	if (!IsValid(HealthSet))
	{
		UE_LOG(LogTemp, Error, TEXT("HealthSet (UGRHealthAttributeSet) is INVALID"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetShieldAttribute()).AddUObject(this, &ThisClass::OnShieldChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetMaxShieldAttribute()).AddUObject(this, &ThisClass::OnMaxShieldChanged);


	float Health = HealthSet->GetHealth();
	float MaxHealth = HealthSet->GetMaxHealth();
	float Shield = HealthSet->GetShield();
	float MaxShield = HealthSet->GetMaxShield();

	UpdatePlayerHealth(Health);
	UpdatePlayerMaxHealth(MaxHealth);
	UpdatePlayerShield(Shield);
	UpdatePlayerMaxShield(MaxShield);

	GetWorldTimerManager().SetTimer(OtherPlayerStatusUpdateTimer, this, &ThisClass::OnUpdateOtherPlayerStatus, OtherPlayerStatusUpdateInterval, true);
}

void AGRBattlePlayerController::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerHealth(Data.NewValue);
}

void AGRBattlePlayerController::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerMaxHealth(Data.NewValue);
}

void AGRBattlePlayerController::OnShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerShield(Data.NewValue);
}

void AGRBattlePlayerController::OnMaxShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerMaxShield(Data.NewValue);
}
