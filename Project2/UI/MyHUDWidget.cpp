// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyHUDWidget.h"
#include "MyMatchTimerWidget.h"
#include "MyCurrentScoreWidget.h"

UMyHUDWidget::UMyHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MatchTimer = Cast<UMyMatchTimerWidget>(GetWidgetFromName(TEXT("WidgetTimer")));
	ensure(MatchTimer);

	CurrentScore = Cast<UMyCurrentScoreWidget>(GetWidgetFromName(TEXT("WidgetCurrentScore")));
	ensure(CurrentScore);
}
