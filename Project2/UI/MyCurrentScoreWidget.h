// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyCurrentScoreWidget.generated.h"

/**
 * 플레이어 HUD에 현재 점수 표시
 */
UCLASS()
class PROJECT2_API UMyCurrentScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyCurrentScoreWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateKillPoint(int32 NewKillPoint);
	void UpdatePlayPoint(int32 NewPlayPoint);
	void UpdateStealPoint(int32 NewStealPoint);

	FString GetPointText(int32 CurrentPoint);

protected:
	UPROPERTY()
		TObjectPtr<class UTextBlock> KillPoint;

	UPROPERTY()
		TObjectPtr<class UTextBlock> PlayPoint;

	UPROPERTY()
		TObjectPtr<class UTextBlock> StealPoint;

	int32 CurrentKillPoint;
	int32 CurrentPlayPoint;
	int32 CurrentStealPoint;
	
};
