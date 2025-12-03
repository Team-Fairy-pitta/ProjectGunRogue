#pragma once

#include "Components/ActorComponent.h"
#include "GRCharacterAttachment.h"
#include "GRAttachmentComponent.generated.h"

class USkeletalMeshComponent;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRAttachmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|CharacterAttachment")
	FGRCharacterAttachmentHandle AddCharacterAttachment(const FGRCharacterAttachment& NewAttachment);

	UFUNCTION(BlueprintCallable, Category = "GunRogue|CharacterAttachment")
	void RemoveCharacterAttachment(FGRCharacterAttachmentHandle Handle);

	USkeletalMeshComponent* GetParentMeshComponent() const;
	USceneComponent* GetSceneComponentToAttachTo() const;

	UPROPERTY(Replicated, Transient)
	FGRCharacterAttachmentList AppliedCharacterAttachmentList;
};
