// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Project2/Interface/MyGameInterface.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnEndMatchDelegate)

UCLASS()
class PROJECT2_API AMyGameMode : public AGameMode, public IMyGameInterface
{
	GENERATED_BODY()

public:
	AMyGameMode();

	virtual FTransform GetRandomStartTransform() const;
	/** Add player's score when player killed other player */
	virtual void OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn);

	virtual void StartPlay() override;
	
public:
	FOnEndMatchDelegate OnEndMatchDelegate;

protected:
	virtual void PostInitializeComponents() override;
	virtual void DefaultGameTimer();
	/** Broadcast a match has end */
	void FinishMatch();

	FTimerHandle GameTimerHandle;

protected:
	TArray<TObjectPtr<class APlayerStart>> PlayerStartArray;

public:
	/** Seamless Travel Section
	캐릭터 재배치 구현 부분 구현중*/
	// 컨트롤러의 폰 저장
	//TObjectPtr<APawn> ControllerPawnToCopy;

	// ControllerPawnArray에 각 old controller의 폰을 저장
	//virtual void PostSeamlessTravel() override;
	// 새 레벨의 컨트롤러에 예전 폰 그대로 연결
	//virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

};
