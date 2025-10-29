#include "TestKeyMapping/TestKeyMappingCharacter.h"
#include "TestKeyMapping/TestKeyMappingPC.h"
#include "GunRogue/Input/GRInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include "Math/UnrealMathUtility.h"

ATestKeyMappingCharacter::ATestKeyMappingCharacter()
{
}

void ATestKeyMappingCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATestKeyMappingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 테스트를 위해, 부모의 SetupPlayerInputComponent()를 사용하지 않음
	// Super::SetupPlayerInputComponent(PlayerInputComponent);

	UGRInputComponent* GRInputComponent = Cast<UGRInputComponent>(PlayerInputComponent);
	check(GRInputComponent);

	ATestKeyMappingPC* PlayerController = GetController<ATestKeyMappingPC>();
	check(PlayerController);

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	UEnhancedInputUserSettings* UserSettings = LoadOrCreateUserSettings(LocalPlayer);
	check(UserSettings);

	//RegisterInputMappingContext(UserSettings, DefaultMappingContext);

	//GetAllSavedKeyProfiles(UserSettings);

	//RandomMappingTest(UserSettings);

	//SaveInputUserSettings(UserSettings);

	if (DefaultMappingContext)
	{
		float Priority = 0.0f;
		Subsystem->AddMappingContext(DefaultMappingContext, Priority);
	}
	if (ForwardInputAction)
	{
		GRInputComponent->BindAction(ForwardInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (BackwardInputAction)
	{
		GRInputComponent->BindAction(BackwardInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (RightInputAction)
	{
		GRInputComponent->BindAction(RightInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (LeftInputAction)
	{
		GRInputComponent->BindAction(LeftInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
}

void ATestKeyMappingCharacter::OnInputMove(const FInputActionValue& InputActionValue)
{
	if (IsValid(Controller))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (!FMath::IsNearlyZero(Value.X))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ATestKeyMappingCharacter::RegisterInputMappingContext(UEnhancedInputUserSettings* UserSettings, UInputMappingContext* MappingContext)
{
	check(UserSettings);

	if (DefaultMappingContext)
	{
		UserSettings->RegisterInputMappingContext(DefaultMappingContext);
	}
}

void ATestKeyMappingCharacter::GetAllSavedKeyProfiles(UEnhancedInputUserSettings* UserSettings)
{
	check(UserSettings);

	for (auto& ProfilePair : UserSettings->GetAllSavedKeyProfiles())
	{
		const FGameplayTag& ProfileName = ProfilePair.Key;
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		for (auto& RowPair : Profile->GetPlayerMappingRows())
		{
			const FName& RowName = RowPair.Key;
			const FKeyMappingRow& MappingRow = RowPair.Value;

			for (const FPlayerKeyMapping& KeyMapping : MappingRow.Mappings)
			{
				FString TagString = ProfileName.ToString();
				FString RowString = RowName.ToString();
				FString MappingString = KeyMapping.ToString();
				UE_LOG(LogTemp, Display, TEXT("%s - %s - %s"), *TagString, *RowString, *MappingString);
			}
		}
	}
}

UEnhancedInputUserSettings* ATestKeyMappingCharacter::LoadOrCreateUserSettings(ULocalPlayer * LocalPlayer)
{
	return UEnhancedInputUserSettings::LoadOrCreateSettings(LocalPlayer);
}

void ATestKeyMappingCharacter::SaveInputUserSettings(UEnhancedInputUserSettings* UserSettings)
{
	check(UserSettings);
	UserSettings->SaveSettings();
}

void ATestKeyMappingCharacter::RandomMappingTest(UEnhancedInputUserSettings* UserSettings)
{
	check(UserSettings);

	for (auto& ProfilePair : UserSettings->GetAllSavedKeyProfiles())
	{
		const FGameplayTag& ProfileName = ProfilePair.Key;
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		for (auto& RowPair : Profile->GetPlayerMappingRows())
		{
			const FName& RowName = RowPair.Key;
			const FKeyMappingRow& MappingRow = RowPair.Value;

			for (const FPlayerKeyMapping& KeyMapping : MappingRow.Mappings)
			{
				FString TagString = ProfileName.ToString();
				FString RowString = RowName.ToString();
				FString MappingString = KeyMapping.ToString();
				
				if (RowName.IsEqual(FName(TEXT("Right"))))
				{
					FMapPlayerKeyArgs Args = {};
					Args.MappingName = RowName;
					Args.Slot = EPlayerMappableKeySlot::First;

					int32 RandomNumber = FMath::RandRange(0, 9);
					switch (RandomNumber)
					{
					case 1: Args.NewKey = EKeys::One; break;
					case 2: Args.NewKey = EKeys::Two; break;
					case 3: Args.NewKey = EKeys::Three; break;
					case 4: Args.NewKey = EKeys::Four; break;
					case 5: Args.NewKey = EKeys::Five; break;
					case 6: Args.NewKey = EKeys::Six; break;
					case 7: Args.NewKey = EKeys::Seven; break;
					case 8: Args.NewKey = EKeys::Eight; break;
					case 9: Args.NewKey = EKeys::Nine; break;
					case 0: Args.NewKey = EKeys::Zero; break;
					default: Args.NewKey = EKeys::D; break;
					}
					FGameplayTagContainer FailureReason;
					UserSettings->MapPlayerKey(Args, FailureReason);
				}
			}
		}
	}
}



