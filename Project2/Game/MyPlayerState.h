// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

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

protected:
	int32 KillPoint;
	int32 PlayPoint;

public:
	FORCEINLINE int32 GetKillPoint() const { return KillPoint; }
	FORCEINLINE void SetKillPoint(int32 NewKillPoint) { KillPoint = NewKillPoint; }

	FORCEINLINE int32 GetPlayPoint() const { return PlayPoint; }
	FORCEINLINE void SetPlayPoint(int32 NewPlayPoint) { PlayPoint = NewPlayPoint; }

protected:
	/** Copy info above to make sure it is persistent after seamless travel */
	virtual void CopyProperties(APlayerState* PlayerState);
};
