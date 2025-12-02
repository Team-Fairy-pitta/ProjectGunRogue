#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "Weapon/GRWeaponInstance.h"
#include "GRWeaponActor.generated.h"

class UGRWeaponDefinition;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class GUNROGUE_API AGRWeaponActor : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()

public:
	AGRWeaponActor();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// 무기 초기화
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_InitWeapon(UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance);

	void InitWeapon(UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance);

	// IGRInteractableActor 구현
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;
	virtual void InteractWith(AActor* OtherActor) override;
	virtual void OnOver() override;
	virtual void OnOut() override;

protected:
	// 무기 정의
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunRogue|Weapon")
	TObjectPtr<UGRWeaponDefinition> WeaponDefinition;

	// 스태틱 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	// 무기 정보 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> WeaponInfoWidgetComponent;

	UPROPERTY(Replicated)
	FGRWeaponInstance WeaponInstance;
};
