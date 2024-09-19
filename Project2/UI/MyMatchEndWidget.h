// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyMatchEndWidget.generated.h"

/**
* 제한 시간 종료 후 보여지는 모든 플레이어의 점수
*/
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
