#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GRCharacterAttachment.generated.h"

class UGRAttachmentComponent;


// 캐릭터에게 붙일(Attach) Actor 정보
USTRUCT(BlueprintType)
struct FGRCharacterAttachment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> AttachmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;
};

USTRUCT(BlueprintType)
struct FGRCharacterAttachmentHandle
{
	GENERATED_BODY()

	void Reset()
	{
		AttachmentHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return AttachmentHandle != INDEX_NONE;
	}

	UPROPERTY()
	int32 AttachmentHandle = INDEX_NONE;
};

USTRUCT()
struct FGRAppliedCharacterAttachmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGRCharacterAttachment Attachment;

	UPROPERTY(NotReplicated)
	int32 AttachmentHandle = INDEX_NONE;

	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};


USTRUCT(BlueprintType)
struct FGRCharacterAttachmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	// FFastArraySerializer 관련 함수
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGRAppliedCharacterAttachmentEntry, FGRCharacterAttachmentList>(Entries, DeltaParms, *this);
	}

	FGRCharacterAttachmentList()
	{
		OwnerComponent = nullptr;
	}
	FGRCharacterAttachmentList(UGRAttachmentComponent* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}

	FGRCharacterAttachmentHandle AddEntry(FGRCharacterAttachment NewAttachment);
	void RemoveEntry(FGRCharacterAttachmentHandle Handle);

	bool SpawnActorForEntry(FGRAppliedCharacterAttachmentEntry& Entry);
	bool DestroyActorForEntry(FGRAppliedCharacterAttachmentEntry& Entry);

	UPROPERTY()
	TArray<FGRAppliedCharacterAttachmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UGRAttachmentComponent> OwnerComponent;

	int32 AttachmentHandleCounter = 0;
};

// FFastArraySerializer를 위해 명시적으로 작성해야 하는 코드
template<>
struct TStructOpsTypeTraits<FGRCharacterAttachmentList> : public TStructOpsTypeTraitsBase2<FGRCharacterAttachmentList>
{
	enum { WithNetDeltaSerializer = true };
};
