// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GRAICharacter.generated.h"

UCLASS()
class GUNROGUE_API AGRAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGRAICharacter();

protected:
	virtual void BeginPlay() override;


};
