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
	SphereComponent->SetupAttachment(SceneRoot);

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

	if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
	{
		const FBoxSphereBounds Bounds = StaticMeshComponent->Bounds;
		const FVector Extents = Bounds.BoxExtent;

		OverlapRadius = Extents.GetMax() + 10;

		if (OverlapRadius > 0.f)
		{
			SphereComponent->SetSphereRadius(OverlapRadius, true);
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

	PlaceActorOnGround();
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

void AGRGoodsActor::PlaceActorOnGround()
{
	FVector NewLocation = GetGroundPointUsingLineTrace();
	this->SetActorLocation(NewLocation);
}

FVector AGRGoodsActor::GetGroundPointUsingLineTrace()
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

	Result = AdjustForOverlap(Result);
	
	return Result;
}

FVector AGRGoodsActor::AdjustForOverlap(const FVector& TargetPos)
{
	FCollisionShape Capsule = FCollisionShape::MakeSphere(OverlapRadius);

	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		TargetPos,
		FQuat::Identity,
		ECC_Visibility,
		Capsule,
		Params
	);

	if (!bHasOverlap)
	{
		return TargetPos;
	}

	for (int32 i = 0; i < 8; i++)
	{
		FVector RandOffset = FVector(
			FMath::FRandRange(-20.0f, 20.0f),
			FMath::FRandRange(-20.0f, 20.0f),
			0.f
		);

		FVector NewPos = TargetPos + RandOffset;

		bool bStillOverlap = GetWorld()->OverlapMultiByChannel(
			Overlaps,
			NewPos,
			FQuat::Identity,
			ECC_Visibility,
			Capsule,
			Params
		);

		if (!bStillOverlap)
		{
			return NewPos;
		}
	}

	return TargetPos;
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



