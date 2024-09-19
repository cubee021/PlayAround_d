// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyHUDWidget.generated.h"

/**
 * Player HUD. All visual widgets for player will be set here
 */
UCLASS()
class PROJECT2_API UMyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
		TObjectPtr<class UMyMatchTimerWidget> MatchTimer;

	UPROPERTY()
		TObjectPtr<class UMyCurrentScoreWidget> CurrentScore;
};
