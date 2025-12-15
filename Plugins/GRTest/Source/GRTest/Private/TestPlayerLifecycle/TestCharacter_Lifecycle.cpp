#include "TestPlayerLifecycle/TestCharacter_Lifecycle.h"

#include "TestPlayerLifecycle/TestPlayerController_Lifecycle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ATestCharacter_Lifecycle::ATestCharacter_Lifecycle()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

void ATestCharacter_Lifecycle::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// 이동 불가
	GetCharacterMovement()->DisableMovement();

	// 입력을 막고 싶으면
	AController* PC = GetController();
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}

	// 충돌 제거
	SetActorEnableCollision(false);

	// 관전 모드 실행
	ATestPlayerController_Lifecycle* TestPC = Cast<ATestPlayerController_Lifecycle>(PC);
	if (TestPC)
	{
		TestPC->Spectating();
	}

}

void ATestCharacter_Lifecycle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestCharacter_Lifecycle, bIsDead);
}

