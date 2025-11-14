#pragma once

#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRItemActor.generated.h"

class UBoxComponent;
class UGRItemDefinition;
class UStaticMeshComponent;

// Item Activate Handle 구조체
// Item을 플레이어가 장착했을 때, 해제했을 때를 관리하기 위한 핸들 구조체 입니다.
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRItemHandle
{
	GENERATED_BODY()

public:
	void EquipItem(UGRAbilitySystemComponent* ASC, UGRItemDefinition* InItemDefinition);
	void UnequipItem();

	UPROPERTY()
	FGRAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY()
	UGRAbilitySystemComponent* CachedASC;

	UPROPERTY()
	UGRItemDefinition* ItemDefinition;
};

// Item Actor
// 바닥(=맵, 레벨)에 떨어져 있는 아이템을 나타내기 위한 Actor입니다.
UCLASS()
class GUNROGUE_API AGRItemActor : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()
	
public:
	AGRItemActor();
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_InitItem(UGRItemDefinition* InItemDefinition);

	// IGRInteractableActor
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;
	virtual void InteractWith(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UGRItemDefinition> ItemDefinition;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
