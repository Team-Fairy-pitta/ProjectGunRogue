// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventoryWidget.h"

#include "GRInventorySlot.h"
#include "Components/WrapBox.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Player/GRPlayerState.h"

void UGRInventoryWidget::UpdateInventoryDisplay()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		return;
	}
	
	AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
	if (!IsValid(PS))
	{
		return;
	}
	TArray<FGRItemHandle>& ItemHandles = PS->GetItemHandles();

	
	for (int32 Index = AllItemSlots.Num() - 1; Index >= 0; --Index)
	{
		UGRInventorySlot* CurrentSlot = AllItemSlots[Index];
		
		if (Index >= ItemHandles.Num())
		{
			if (CurrentSlot)
			{
				CurrentSlot->OnSlotRightClicked.RemoveDynamic(this, &ThisClass::HandleSlotRightClick);
				
				if (ItemSlotsContainer)
				{
					ItemSlotsContainer->RemoveChild(CurrentSlot);
				}
				
				CurrentSlot->RemoveFromParent(); 
			}
			AllItemSlots.RemoveAt(Index);
            
			UE_LOG(LogTemp, Display, TEXT("슬롯 번호 %d 제거"), Index);
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
					UE_LOG(LogTemp, Display, TEXT("새로운 슬롯 추가 번호 : %d"), Index);

					CurrentSlot->OnSlotRightClicked.AddDynamic(this, &ThisClass::HandleSlotRightClick);
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
				UE_LOG(LogTemp, Display, TEXT("아이템 번호: %d"),Index);

				ItemSlot->OnSlotRightClicked.AddDynamic(this, &ThisClass::HandleSlotRightClick);

				UE_LOG(LogTemp, Warning, TEXT("슬롯 번호 %d 에 RightClick 델리게이트 바인딩 성공."), ItemSlot->SlotIndex);
			}
		}
	}
}

void UGRInventoryWidget::NativeDestruct()
{
	for (UGRInventorySlot* ItemSlot : AllItemSlots)
	{
		if (ItemSlot)
		{
			ItemSlot->OnSlotRightClicked.RemoveDynamic(this, &ThisClass::HandleSlotRightClick);
		}
	}
	
	Super::NativeDestruct();
}

void UGRInventoryWidget::HandleSlotRightClick(int32 ClickedSlotIndex)
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		return;
	}
	AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
	if (!IsValid(PS))
	{
		return;
	}
	if (PS)
	{
		PS->UnequipItem(ClickedSlotIndex);
		UpdateInventoryDisplay();
	}
	UE_LOG(LogTemp, Warning, TEXT("슬롯 인덱스 %d 에서 우클릭!"), ClickedSlotIndex);
}
