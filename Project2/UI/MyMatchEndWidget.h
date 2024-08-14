// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyMatchEndWidget.generated.h"

UCLASS()
class PROJECT2_API UMyMatchEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMyMatchEndWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
		TObjectPtr<class UUniformGridPanel> GridSlot;

public:
	/** Show screen with player's end scores */
	void SetFinalScore();
};
