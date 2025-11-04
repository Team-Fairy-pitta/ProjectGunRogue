#include "TestCheatManager/TestCheatManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TestCheatManager/TestGRCharacter_Cheat.h"
#include "TestCheatManager/TestItemDataAsset_Cheat.h"
#include "TestCheatManager/TestLocationDataAsset.h"

#if WITH_EDITOR
#include "PackageTools.h"
#include "ObjectTools.h"
#include "Editor.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#endif


void UTestCheatManager::Flying()
{
	ACharacter* MyCharacter = Cast<ACharacter>(GetOuterAPlayerController()->GetPawn());
	if (!MyCharacter) return;

	UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement();
	if (!MoveComp) return;

	if (MoveComp->MovementMode != MOVE_Flying)
	{
		MoveComp->SetMovementMode(MOVE_Flying);
		MoveComp->GravityScale = 0.0f;
		MoveComp->BrakingFrictionFactor = 10.0f;
		MoveComp->BrakingDecelerationFlying = 2048.0f;
		UE_LOG(LogTemp, Log, TEXT("비행 모드"));	
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("현재 비행 모드"));
	}
}

void UTestCheatManager::Walking()
{
	ACharacter* MyCharacter = Cast<ACharacter>(GetOuterAPlayerController()->GetPawn());
	if (!MyCharacter) return;

	UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement();
	if (!MoveComp) return;

	if (MoveComp->MovementMode != MOVE_Walking)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->GravityScale = 1.0f;
		UE_LOG(LogTemp, Log, TEXT("걷기 모드"));
	}
	{
		UE_LOG(LogTemp, Log, TEXT("현재 걷기 모드"));
	}
}

void UTestCheatManager::SelfDamage(float DamageAmount)
{
	// 예시: 캐릭터의 체력을 감소시키는 로직
	//ACustomCharacter* CustomChar = Cast<ACustomCharacter>(GetOuterAPlayerController()->GetPawn());
	//if (CustomChar)
	//{
	//	CustomChar->TakeDamage(DamageAmount);
	//}
}

void UTestCheatManager::SelfHeal(float HealAmount)
{
	// 예시: 캐릭터의 체력을 회복시키는 로직
	//ACustomCharacter* CustomChar = Cast<ACustomCharacter>(GetOuterAPlayerController()->GetPawn());
	//if (CustomChar)
	//{
		//CustomChar->Heal(HealAmount);
	//}
}

void UTestCheatManager::SpawnItem(FString ItemName, float Distance)
{
	if (!ItemDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Cheat]: ItemDataAsset is not assigned!"));
		return;
	}

	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	APawn* MyPawn = PC->GetPawn();
	if (!MyPawn) return;

	FName SearchName(*ItemName);

	if (Distance <= 0.f)
	{
		Distance = 0.f;
	}

	for (const FItemData& Data : ItemDataAsset->Items)
	{
		if (Data.ItemName == SearchName)
		{
			if (!Data.ItemClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Cheat]: Item '%s' has no ItemClass assigned!"), *ItemName);
				return;
			}

			UWorld* World = MyPawn->GetWorld();
			const FVector Forward = MyPawn->GetActorForwardVector();
			const FVector SpawnLoc = MyPawn->GetActorLocation() + Forward * Distance * 100;
			const FRotator SpawnRot = MyPawn->GetActorRotation();

			AActor* Spawned = World->SpawnActor<AActor>(Data.ItemClass, SpawnLoc, SpawnRot);
			if (Spawned)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Cheat]: Spawned item '%s' at %s"), *ItemName, *SpawnLoc.ToString());
			}
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Cheat]: Item '%s' not found in DataAsset!"), *ItemName);
}

void UTestCheatManager::TP(FString LocationName)
{
	const FNamedLocation* Found = LocationDataAsset->Locations.FindByPredicate(
		[&](const FNamedLocation& Loc) { return Loc.LocationName == LocationName; });

	if (Found && GetOuterAPlayerController())
	{
		if (APawn* Pawn = GetOuterAPlayerController()->GetPawn())
		{
			Pawn->SetActorLocationAndRotation(Found->Location, Found->Rotation);
		}
	}
}

void UTestCheatManager::SaveCurrentLocation(FName LocationName)
{
	if (!LocationDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("LocationDataAsset not assigned!"));
		return;
	}

	APlayerController* PC = GetOuterAPlayerController();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	FVector CurrentLocation = Pawn->GetActorLocation();
	FRotator CurrentRotation = FRotator(0.0f, 0.0f, 0.0f); //Pawn->GetActorRotation();

	if (LocationDataAsset->Locations.ContainsByPredicate([&](const FNamedLocation& Loc) { return Loc.LocationName == LocationName; }))
	{
		UE_LOG(LogTemp, Warning, TEXT("Location with this name already exists!"));
		return;
	}

	FNamedLocation NewLocation;
	NewLocation.LocationName = LocationName;
	NewLocation.Location = CurrentLocation;
	NewLocation.Rotation = CurrentRotation;

	LocationDataAsset->Locations.Add(NewLocation);
	LocationDataAsset->MarkPackageDirty();

#if WITH_EDITOR
	UPackage* Package = LocationDataAsset->GetOutermost();
	if (Package)
	{
		FString PackageFileName = FPackageName::LongPackageNameToFilename(
			Package->GetName(),
			FPackageName::GetAssetPackageExtension()
		);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		SaveArgs.bForceByteSwapping = false;
		SaveArgs.bWarnOfLongFilename = true;

		bool bSaved = UPackage::SavePackage(Package, LocationDataAsset, *PackageFileName, SaveArgs);
		if (bSaved)
		{
			UE_LOG(LogTemp, Warning, TEXT("Saved LocationDataAsset to %s"), *PackageFileName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save LocationDataAsset!"));
		}
	}
#endif
}
