#include "Character/Attachment/GRCharacterAttachment.h"
#include "Character/Attachment/GRAttachmentComponent.h"

void FGRCharacterAttachmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FGRAppliedCharacterAttachmentEntry& AttachmentEntry = Entries[Index];
		DestroyActorForEntry(AttachmentEntry);
	}
}

void FGRCharacterAttachmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FGRAppliedCharacterAttachmentEntry& AttachmentEntry = Entries[Index];
		SpawnActorForEntry(AttachmentEntry);
	}
}

void FGRCharacterAttachmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FGRAppliedCharacterAttachmentEntry& AttachmentEntry = Entries[Index];
		DestroyActorForEntry(AttachmentEntry);
		SpawnActorForEntry(AttachmentEntry);
	}
}

FGRCharacterAttachmentHandle FGRCharacterAttachmentList::AddEntry(FGRCharacterAttachment NewAttachment)
{
	FGRCharacterAttachmentHandle Result;
	Result.AttachmentHandle = AttachmentHandleCounter++;

	if (!IsValid(OwnerComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerComponent is INVALID"));
		return Result;
	}

	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerComponent->GetOwner() is INVALID"));
		return Result;
	}

	if (OwnerActor->HasAuthority())
	{
		FGRAppliedCharacterAttachmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.AttachmentHandle = Result.AttachmentHandle;
		NewEntry.Attachment = NewAttachment;

		SpawnActorForEntry(NewEntry);
		MarkItemDirty(NewEntry);
	}
	return Result;
}

void FGRCharacterAttachmentList::RemoveEntry(FGRCharacterAttachmentHandle Handle)
{
	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		FGRAppliedCharacterAttachmentEntry& Entry = *EntryIterator;
		if (Entry.AttachmentHandle == Handle.AttachmentHandle)
		{
			DestroyActorForEntry(Entry);
			EntryIterator.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}

bool FGRCharacterAttachmentList::SpawnActorForEntry(FGRAppliedCharacterAttachmentEntry& Entry)
{
	if (!IsValid(OwnerComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerComponent is INVALID"));
		return false;
	}

	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerComponent->GetOwner() is INVALID"));
		return false;
	}

	bool bIsDedicatedServer = OwnerComponent->IsNetMode(NM_DedicatedServer);
	if (bIsDedicatedServer)
	{
		UE_LOG(LogTemp, Display, TEXT("DedicatedServer: Do Not Spawn Attachment Actor"));
		return false;
	}

	if (Entry.Attachment.AttachmentClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Entry.Attachment.AttachmentClass is INVALID"));
		return false;
	}
	
	USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo();
	if (!ComponentToAttachTo)
	{
		UE_LOG(LogTemp, Error, TEXT("EComponentToAttachTo is INVALID"));
		return false;
	}

	const FName SocketName = Entry.Attachment.SocketName;
	const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(SocketName);

	AActor* PartOuter = OwnerComponent->GetOwner();
	UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(PartOuter);
	if (!PartComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT create is PartComponent"));
		return false;
	}

	PartComponent->SetupAttachment(ComponentToAttachTo, SocketName);
	PartComponent->SetChildActorClass(Entry.Attachment.AttachmentClass);
	PartComponent->RegisterComponent();

	// 스폰된 Actor가 parent보다 먼저 Tick()하지 않도록, 선행 조건을 붙인다.
	if (AActor* SpawnedActor = PartComponent->GetChildActor())
	{
		if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
		{
			SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
		}
	}
	Entry.SpawnedComponent = PartComponent;

	return true;
}

bool FGRCharacterAttachmentList::DestroyActorForEntry(FGRAppliedCharacterAttachmentEntry& Entry)
{
	if (Entry.SpawnedComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entry.SpawnedComponent is NULLPTR"));
		return false;
	}

	Entry.SpawnedComponent->DestroyComponent();
	Entry.SpawnedComponent = nullptr;
	return true;
}
