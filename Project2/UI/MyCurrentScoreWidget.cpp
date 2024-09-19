// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyCurrentScoreWidget.h"
#include "Components/TextBlock.h"
#include "Interface/MyCurrentScoreWidgetInterface.h"

UMyCurrentScoreWidget::UMyCurrentScoreWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentKillPoint = 0;
	CurrentPlayPoint = 0;
	CurrentStealPoint = 0;
}

void UMyCurrentScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KillPoint = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtKilllPoint")));
	ensure(KillPoint);

	PlayPoint = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtPlayPoint")));
	ensure(PlayPoint);

	StealPoint = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtStealPoint")));
	ensure(StealPoint);

	IMyCurrentScoreWidgetInterface* ScorePawn = Cast<IMyCurrentScoreWidgetInterface>(GetOwningPlayerPawn());
	if (ScorePawn)
	{
		ScorePawn->UpdateCurrentScoreWidget(this);
	}

}

void UMyCurrentScoreWidget::UpdateKillPoint(int32 NewKillPoint)
{
	CurrentKillPoint = NewKillPoint;

	if (KillPoint)
	{
		KillPoint->SetText(FText::FromString(GetPointText(CurrentKillPoint)));
	}
}

void UMyCurrentScoreWidget::UpdatePlayPoint(int32 NewPlayPoint)
{
	CurrentPlayPoint = NewPlayPoint;

	if (PlayPoint)
	{
		PlayPoint->SetText(FText::FromString(GetPointText(CurrentPlayPoint)));
	}
}

void UMyCurrentScoreWidget::UpdateStealPoint(int32 NewStealPoint)
{
	CurrentStealPoint = NewStealPoint;

	if (StealPoint)
	{
		StealPoint->SetText(FText::FromString(GetPointText(CurrentStealPoint)));
	}
}

FString UMyCurrentScoreWidget::GetPointText(int32 CurrentPoint)
{
	return FString::Printf(TEXT("%d"), CurrentPoint);
}
