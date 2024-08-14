// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"
#include "MyMatchTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyMatchTimerWidget : public UMyUserWidget
{
	GENERATED_BODY()

public:
	UMyMatchTimerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateTimer(float NewTime);
	FString GetTimerText();

protected:
	UPROPERTY()
		TObjectPtr<class UTextBlock> Timer;

	UPROPERTY()
		float CurrentTime;
	
};
