// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/GRAttackAnimNotify_GameplayEvent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UGRAttackAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// AbilitySystemInterface를 구현한 액터인지 확인
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (!ASI) return;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;

	// 이벤트 데이터 생성
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = Owner;
	// 필요하면 추가 정보 설정 가능

	// GameplayEvent 보내기
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, EventData);
}
