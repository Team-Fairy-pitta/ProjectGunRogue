#include "Character/Attachment/GRAttachmentComponent.h"
#include "Character/GRCharacter.h"
#include "Net/UnrealNetwork.h"

UGRAttachmentComponent::UGRAttachmentComponent(const FObjectInitializer& ObjectInitialize)
	: Super(ObjectInitialize), AppliedCharacterAttachmentList(this)
{
	SetIsReplicatedByDefault(true);
}

void UGRAttachmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AppliedCharacterAttachmentList);
}

FGRCharacterAttachmentHandle UGRAttachmentComponent::AddCharacterAttachment(const FGRCharacterAttachment& NewAttachment)
{
	return AppliedCharacterAttachmentList.AddEntry(NewAttachment);
}

void UGRAttachmentComponent::RemoveCharacterAttachment(FGRCharacterAttachmentHandle Handle)
{
	AppliedCharacterAttachmentList.RemoveEntry(Handle);
}

USkeletalMeshComponent* UGRAttachmentComponent::GetParentMeshComponent() const
{
	ACharacter* OwnerCharacter = GetOwner<ACharacter>();
	if (IsValid(OwnerCharacter))
	{
		return OwnerCharacter->GetMesh();
	}
	return nullptr;
}

USceneComponent* UGRAttachmentComponent::GetSceneComponentToAttachTo() const
{
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetParentMeshComponent())
	{
		return SkeletalMeshComponent;
	}
	else if (AActor* Owner = GetOwner())
	{
		return Owner->GetRootComponent();
	}
	else
	{
		return nullptr;
	}
}
