// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/MyCharacterWidgetInterface.h"
#include "Game/MyPlayerState.h"

UMyHpBarWidget::UMyHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UMyHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	HpStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensure(HpStat);

	PlayerId = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtPlayerID")));
	ensure(PlayerId);

	IMyCharacterWidgetInterface* CharacterWidget = Cast<IMyCharacterWidgetInterface>(OwningActor);
	if(CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}

	PlayerId->SetText(FText::FromString(GetPlayerIdText()));

}

void UMyHpBarWidget::UpdateHpBar(float NewCurrentHp, float NewMaxHp)
{
	CurrentHp = NewCurrentHp;
	MaxHp = NewMaxHp;

	ensure(MaxHp > 0.f);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(CurrentHp / MaxHp);
	}

	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStatText()));
	}
}

FString UMyHpBarWidget::GetHpStatText()
{
	return FString::Printf(TEXT("%.0f / %.0f"), CurrentHp, MaxHp);
}

FString UMyHpBarWidget::GetPlayerIdText()
{
	return FString::Printf(TEXT("%s"), *OwningActor->GetName());
}
