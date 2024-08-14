// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Transient, Replicated)
		int32 RemainingTime;

	int32 MatchPlayTime = 150;
	int32 ShowResultWaitingTime = 5;
};
