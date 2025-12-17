// GRGoodsActor.cpp


#include "Goods/GRGoodsActor.h"

#include "GRGoodsDefinition.h"
#include "Character/GRCharacter.h"
#include "Components/SphereComponent.h"
#include "Player/GRPlayerState.h"
#include "Engine/OverlapResult.h"

AGRGoodsActor::AGRGoodsActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneRoot);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(50.f);
	SphereComponent->SetupAttachment(StaticMeshComponent);
	SphereComponent->SetRelativeLocation(FVector::ZeroVector);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGRGoodsActor::OnBeginOverlap);
}

void AGRGoodsActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AActor* GoodsOwner = GetOwner();
		if (GoodsOwner != nullptr)
		{
			AGRPlayerState* GoodsOwnerPlayerState = Cast<AGRPlayerState>(GoodsOwner);
			if (IsValid(GoodsOwnerPlayerState))
			{
				AController* OwnerController = GoodsOwnerPlayerState->GetOwningController();
				if (!OwnerController->IsLocalController())
				{
					SetInvisible();
					return;
				}
			}
		}
	}
	
	if (GoodsDefinition)
	{
		InitGoods();
	}
}

bool AGRGoodsActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	bool DefaultNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

	AActor* GoodsOwner = GetOwner();
	if (GoodsOwner)
	{
		if (RealViewer && RealViewer->IsA<APlayerController>())
		{
			const APlayerController* ViewerController = Cast<APlayerController>(RealViewer);
			if (IsValid(ViewerController))
			{
				bool bIsOwnClient = ViewerController->GetPlayerState<AGRPlayerState>() == GoodsOwner;
				return bIsOwnClient;
			}
		}
	}
	
	return DefaultNetRelevant;
}

void AGRGoodsActor::MulticastRPC_InitGoods_Implementation()
{
	InitGoods();
}

void AGRGoodsActor::InitGoods()
{
	if (!GoodsDefinition)
	{
		return;
	}

	if (!StaticMeshComponent)
	{
		return;
	}

	StaticMeshComponent->SetStaticMesh(GoodsDefinition->GoodsMesh);

	Amount = GoodsDefinition->GoodsAmount;

	if (UStaticMesh* Mesh = StaticMeshComponent->GetStaticMesh())
	{
		const FBoxSphereBounds Bounds = Mesh->GetBounds();
		const FVector Extents = Bounds.BoxExtent;

		const float OverlapRadius = Extents.GetMax() + 10;

		if (OverlapRadius > 0.f)
		{
			SphereComponent->SetSphereRadius(OverlapRadius, true);
		}

		SphereComponent->SetRelativeLocation(Bounds.Origin);

		UE_LOG(LogTemp, Warning, TEXT("OverlapRadius : %.1f"), OverlapRadius);
	}
}

void AGRGoodsActor::SetInvisible()
{
	if (!StaticMeshComponent)
	{
		return;
	}

	StaticMeshComponent->SetVisibility(false, true);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGRGoodsActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	
	AGRCharacter* Character = Cast<AGRCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	AGRPlayerState* HitPlayerState = Character->GetPlayerState<AGRPlayerState>();
	if (!HitPlayerState)
	{
		return;
	}

	AActor* GoodsOwner = GetOwner();
	if (!GoodsOwner)
	{
		return;
	}

	AGRPlayerState* OwnerPlayerState = Cast<AGRPlayerState>(GoodsOwner);
	if (!OwnerPlayerState)
	{
		return;
	}

	if (HitPlayerState != OwnerPlayerState)
	{
		return;
	}
	
	if (!GoodsDefinition)
	{
		return;
	}

	if (GoodsDefinition->GoodsType == FName("Gold"))
	{
		HitPlayerState->AddGold(Amount);
	}
	else if (GoodsDefinition->GoodsType == FName("Gem"))
	{
		HitPlayerState->AddMetaGoods(Amount);
	}

	Destroy();
}



