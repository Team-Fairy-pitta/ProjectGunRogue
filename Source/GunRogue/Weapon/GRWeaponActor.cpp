#include "Weapon/GRWeaponActor.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Item/GRItemInfoWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Net/UnrealNetwork.h"

AGRWeaponActor::AGRWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	WeaponInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponInfoWidgetComponent"));
	WeaponInfoWidgetComponent->SetupAttachment(StaticMeshComponent);
	WeaponInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WeaponInfoWidgetComponent->SetVisibility(false);
	WeaponInfoWidgetComponent->SetDrawSize(FVector2D(400, 300));
}

void AGRWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(WeaponInfoWidgetComponent))
	{
		// TODO: 나중에 무기 전용 Widget으로 변경 필요
		UClass* WidgetClass = LoadClass<UUserWidget>(
			nullptr, TEXT("/Game/GunRogue/Blueprints/Item/WBP_ItemInfoWidget.WBP_ItemInfoWidget_C"));

		if (WidgetClass)
		{
			WeaponInfoWidgetComponent->SetWidgetClass(WidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Path of Blueprint Widget Class (UGRItemInfoWidget) is INVALID"));
		}
	}

	FGRWeaponInstance NewInstance;

	if (WeaponDefinition)
	{
		NewInstance.Validate();
		InitWeapon(WeaponDefinition, NewInstance);
	}

}

void AGRWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WeaponInstance);
}


void AGRWeaponActor::MulticastRPC_InitWeapon_Implementation(UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance)
{
	InitWeapon(InWeaponDefinition, InWeaponInstance);
}

void AGRWeaponActor::InitWeapon(UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance)
{
	WeaponDefinition = InWeaponDefinition;
	WeaponInstance = InWeaponInstance;

	if (!WeaponDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	// 메시 설정
	if (StaticMeshComponent && WeaponDefinition->WeaponPickupMesh)
	{
		StaticMeshComponent->SetStaticMesh(WeaponDefinition->WeaponPickupMesh);
	}

	// 위젯 정보 설정 // TODO: 나중에 무기 전용 Widget으로 변경 필요
	if (WeaponInfoWidgetComponent)
	{
		UWidget* Widget = WeaponInfoWidgetComponent->GetWidget();
		UGRItemInfoWidget* ItemInfoWidget = Cast<UGRItemInfoWidget>(Widget);

		if (ItemInfoWidget)
		{
			ItemInfoWidget->InitItemInfo(
				WeaponDefinition->WeaponIcon,
				WeaponDefinition->WeaponName,
				WeaponDefinition->WeaponDescription
			);
		}
	}
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRWeaponActor::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}

void AGRWeaponActor::InteractWith(AActor* OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter is INVALID"));
		return;
	}

	AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState is INVALID"));
		return;
	}

	if (!WeaponDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	// PlayerState의 무기 장착 시도
	GRPlayerState->TryEquipWeapon(WeaponDefinition, WeaponInstance, this);
}

void AGRWeaponActor::OnOver()
{
	if (WeaponInfoWidgetComponent)
	{
		WeaponInfoWidgetComponent->SetVisibility(true);
	}
}

void AGRWeaponActor::OnOut()
{
	if (WeaponInfoWidgetComponent)
	{
		WeaponInfoWidgetComponent->SetVisibility(false);
	}
}
