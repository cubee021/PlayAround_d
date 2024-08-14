// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"
#include "MyHpBarWidget.generated.h"

/**
 * Widget that will be shown on top of each player's head
 */
UCLASS()
class PROJECT2_API UMyHpBarWidget : public UMyUserWidget
{
	GENERATED_BODY()

public:
	UMyHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
		TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
		TObjectPtr<class UTextBlock> HpStat;
	/** Player's controller Id */
	UPROPERTY()
		TObjectPtr<class UTextBlock> PlayerId;

	UPROPERTY()
		float CurrentHp;

	UPROPERTY()
		float MaxHp;


public:
	/** Update HpProgressBar and HpStat when value has changed */
	void UpdateHpBar(float NewCurrentHp, float NewMaxHp);

	FString GetHpStatText();
	FString GetPlayerIdText();

	
};
