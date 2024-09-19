// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnKillPointChangeDelegate, int32 /*NewKillPoint*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayPointChangeDelegate, int32 /*NewKillPoint*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStealPointChangeDelegate, int32 /*NewKillPoint*/);

/**
 * Scores player can gain during game play
 * Consist of 4 parts 
 * 1. Kill Point : get score when kill other players
 * 2. Play Point : get score when interact with objects in playground
 * 3. Steal Point : get score when player gives certain object to other player.
 *					steal current total score of other player
 */
UCLASS()
class PROJECT2_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMyPlayerState();

	FOnKillPointChangeDelegate OnKillPointChange;
	FOnPlayPointChangeDelegate OnPlayPointChange;
	FOnStealPointChangeDelegate OnStealPointChange;

protected:
	int32 KillPoint;
	int32 PlayPoint;
	int32 StealPoint;

public:
	FORCEINLINE int32 GetKillPoint() const { return KillPoint; }
	FORCEINLINE void SetKillPoint(int32 NewKillPoint) { KillPoint = NewKillPoint; OnKillPointChange.Broadcast(KillPoint); }

	FORCEINLINE int32 GetPlayPoint() const { return PlayPoint; }
	FORCEINLINE void SetPlayPoint(int32 NewPlayPoint) { PlayPoint = NewPlayPoint; OnPlayPointChange.Broadcast(PlayPoint); }

	FORCEINLINE int32 GetStealPoint() const { return StealPoint; }
	FORCEINLINE void SetStealPoint(int32 NewStealPoint) { StealPoint = NewStealPoint; OnStealPointChange.Broadcast(StealPoint); }

protected:
	/** Copy info above to make sure it is persistent after seamless travel */
	virtual void CopyProperties(APlayerState* PlayerState);
};
