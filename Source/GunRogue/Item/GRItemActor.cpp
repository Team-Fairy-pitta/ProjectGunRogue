#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Item/GRItemInfoWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

void FGRItemHandle::EquipItem(UGRAbilitySystemComponent* ASC, UGRItemDefinition* InItemDefinition)
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}
	if (!IsValid(InItemDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition (UGRItemDefinition) is INVALID"));
		return;
	}
	if (!IsValid(InItemDefinition->AbilitySet))
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition->AbilitySet (UGRAbilitySet) is INVALID"));
		return;
	}

	CachedASC = ASC;
	ItemDefinition = InItemDefinition;
	ItemDefinition->AbilitySet->GiveToAbilitySystem(CachedASC, &GrantedHandles);
}

void FGRItemHandle::UnequipItem()
{
	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}

	GrantedHandles.TakeFromAbilitySystem(CachedASC);
}


AGRItemActor::AGRItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	
	SetRootComponent(StaticMeshComponent);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneRoot);

	FTransform MeshTransform;
	MeshTransform.SetLocation(FVector(43.0f, 0.0f, 40.3f)); /* 아이템 Mesh 의 크기를 이용해 구한 상수값 */
	MeshTransform.SetRotation(FQuat::MakeFromRotator(FRotator(30.0f, 0, 0)));
	StaticMeshComponent->SetWorldTransform(MeshTransform);

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	SphereMesh->SetupAttachment(StaticMeshComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 35.0f));
		SphereMesh->SetWorldScale3D(FVector(0.9f));
	}

	ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
	ItemInfoWidgetComponent->SetupAttachment(SceneRoot);
	ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ItemInfoWidgetComponent->SetVisibility(false);
	ItemInfoWidgetComponent->SetDrawSize(FVector2D(400, 300)); // Desired Size of UGRItemInfoWidget
}

void AGRItemActor::BeginPlay()
{
	Super::BeginPlay();

	// Listen Server에서, 다른 Client의 Item이 화면에 나타나는 것을 막기 위한 코드
	if (HasAuthority())
	{
		AActor* ItemOwner = GetOwner();
		if (ItemOwner != nullptr)
		{
			AGRPlayerState* ItemOwnerPlayerState = Cast<AGRPlayerState>(ItemOwner);
			if (IsValid(ItemOwnerPlayerState))
			{
				AController* OwnerController = ItemOwnerPlayerState->GetOwningController();
				if (!OwnerController->IsLocalPlayerController())
				{
					SetInvisibile();
					return;
				}
			}
		}
	}

	if (IsValid(ItemInfoWidgetComponent))
	{
		if (ItemInfoWidgetClass)
		{
			ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ItemInfoWidgetComponent is INVALID"));
		}
	}

	if (ItemDefinition)
	{
		InitItem(ItemDefinition);
	}
}

bool AGRItemActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool DefaultNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

	AActor* ItemOwner = GetOwner();
	if (ItemOwner)
	{
		if (RealViewer && RealViewer->IsA<APlayerController>())
		{
			const APlayerController* ViewerController = Cast<APlayerController>(RealViewer);
			if (IsValid(ViewerController))
			{
				bool bIsOwnClient = ViewerController->GetPlayerState<AActor>() == ItemOwner;
				return bIsOwnClient && DefaultNetRelevant;
			}
		}
	}
	return DefaultNetRelevant;
}

void AGRItemActor::MulticastRPC_InitItem_Implementation(UGRItemDefinition* InItemDefinition, EGRItemPlacement ItemPlacement)
{
	InitItem(InItemDefinition);
	if (ItemPlacement == EGRItemPlacement::GROUND)
	{
		PlaceActorOnGround();
	}
}

void AGRItemActor::InitItem(UGRItemDefinition* InItemDefinition)
{
	ItemDefinition = InItemDefinition;

	if (!ItemDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition is INVALID"));
		return;
	}

	if (!StaticMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMeshComponent is INVALID"));
		return;
	}

	if (!SphereMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("SphereMesh is INVALID"));
		return;
	}

	if (ItemDefinition)
	{
		StaticMeshComponent->SetStaticMesh(ItemDefinition->ItemMesh);
	}

	if (ItemInfoWidgetComponent)
	{
		UWidget* Widget = ItemInfoWidgetComponent->GetWidget();
		UGRItemInfoWidget* ItemInfoWidget = Cast<UGRItemInfoWidget>(Widget);
		if (ItemInfoWidget)
		{
			UTexture2D* ItemIcon = ItemDefinition->ItemIcon;
			const FText& ItemName = ItemDefinition->ItemName;
			const FText& ItemDescription = ItemDefinition->ItemDescription;
			ItemInfoWidget->InitItemInfo(ItemIcon, ItemName, ItemDescription);
		}
	}

	switch (ItemDefinition->Rarity)
	{
	case EItemRarity::NORMAL:
		SphereMesh->SetMaterial(0, RarityMaterial_Normal);
		break;
	case EItemRarity::RARE:
		SphereMesh->SetMaterial(0, RarityMaterial_Rare);
		break;
	case EItemRarity::EPIC:
		SphereMesh->SetMaterial(0, RarityMaterial_Epic);
		break;
	default:
		SphereMesh->SetVisibility(false);
		break;
	}
}

void AGRItemActor::SetInvisibile()
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetVisibility(false, true);
	}
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRItemActor::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}

void AGRItemActor::InteractWith(AActor* OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter (AGRCharacter) is INVALID"));
		return;
	}

	AGRPlayerState* GRPlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	if (!ItemDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition (UGRItemDefinition) is INVALID"));
		return;
	}

	GRPlayerState->TryEquipItem(ItemDefinition, this);
}

void AGRItemActor::OnOver()
{
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetVisibility(true);
	}
}

void AGRItemActor::OnOut()
{
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetVisibility(false);
	}
}

bool AGRItemActor::CanInteract(AActor* OtherActor)
{
	AActor* ItemOwner = GetOwner();
	if (ItemOwner == nullptr)
	{
		return true;
	}

	if (OtherActor->IsA<AGRCharacter>())
	{
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
		if (IsValid(GRCharacter))
		{
			AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
			if (IsValid(GRPlayerState))
			{
				return ItemOwner == GRPlayerState;
			}
		}
	}
	else if (OtherActor->IsA<AGRPlayerState>())
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(OtherActor);
		if (IsValid(GRPlayerState))
		{
			return ItemOwner == GRPlayerState;
		}
	}
	return false;
}

void AGRItemActor::PlaceActorOnGround()
{
	FVector NewLocation = GetGroundPointUsingLineTrace();
	this->SetActorLocation(NewLocation);
}

FVector AGRItemActor::GetGroundPointUsingLineTrace()
{
	if (!GetWorld())
	{
		return this->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;
	FVector Start = this->GetActorLocation();
	FVector Result = Start;
	FVector End = Start + FallDirection * (CheckDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}
	
	return Result;
}

