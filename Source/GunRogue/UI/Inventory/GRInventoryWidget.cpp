// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventoryWidget.h"

#include "GRInventorySlot.h"
#include "Components/WrapBox.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Player/GRPlayerState.h"

void UGRInventoryWidget::UpdateInventoryDisplay(const TArray<FGRItemHandle>& ItemHandles)
{
	for (UGRInventorySlot* SlotItems : AllItemSlots)
	{
		if (SlotItems)
		{
			SlotItems->ClearSlot();
		}
	}

	for (int32 Index = 0; Index < ItemHandles.Num(); ++Index)
	{
		UGRInventorySlot* CurrentSlot = nullptr;
		const FGRItemHandle& ItemHandle = ItemHandles[Index];
		
		if (AllItemSlots.IsValidIndex(Index))
		{
			CurrentSlot = AllItemSlots[Index];
		}
		else
		{
			if (InventorySlotClass.Get() && ItemSlotsContainer) 
			{
				CurrentSlot = CreateWidget<UGRInventorySlot>(GetOwningPlayer(), InventorySlotClass);

				if (CurrentSlot)
				{
					ItemSlotsContainer->AddChild(CurrentSlot); 
					
					AllItemSlots.Add(CurrentSlot);
					
					CurrentSlot->SetSlotIndex(Index);
					UE_LOG(LogTemp, Display, TEXT("새로운 슬롯 추가됨 인덱스: %d"), Index);
				}
			}
		}
		if (CurrentSlot && ItemHandle.ItemDefinition)
		{
			UGRItemDefinition* Definition = ItemHandle.ItemDefinition;
			
			CurrentSlot->SetSlotIndex(Index);
          
			FString ItemNameString = Definition->ItemName.ToString();
			UE_LOG(LogTemp, Display, TEXT("아이템 이름: %s 번호 : %d "), *ItemNameString, Index);
			CurrentSlot->SetSlot(Definition->ItemIcon, Definition->ItemName, Definition->ItemDescription);
		}
	}
}

void UGRInventoryWidget::Active()
{
	APlayerController* PC = GetOwningPlayer();
	AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
	TArray<FGRItemHandle>& Items = PS->GetItemHandles();
	UpdateInventoryDisplay(Items);
	UE_LOG(LogTemp, Display, TEXT("실행됨"));
}

void UGRInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AllItemSlots.Empty();

	if (ItemSlotsContainer)
	{
		for (UWidget* Child : ItemSlotsContainer->GetAllChildren())
		{
			if (UGRInventorySlot* ItemSlot = Cast<UGRInventorySlot>(Child))
			{
				AllItemSlots.Add(ItemSlot);

				int32 Index = AllItemSlots.Num() - 1;
				ItemSlot->SetSlotIndex(Index);
				UE_LOG(LogTemp, Display, TEXT("아이템 이름: %d"),Index);
			}
		}
	}
}
