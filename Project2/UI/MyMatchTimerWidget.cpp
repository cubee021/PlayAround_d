// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMatchTimerWidget.h"
#include "Interface/MyGameInterface.h"
#include "Game/MyGameMode.h"
#include "Game/MyGameState.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UMyMatchTimerWidget::UMyMatchTimerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMyMatchTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Timer = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtTimer")));
	ensure(Timer);
}

void UMyMatchTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ���1 : ���Ӹ�� ���ļ� �ð� ������Ʈ, Ŭ���̾�Ʈ�� ������Ʈ �ȵ�.
	IMyGameInterface* GameMode = Cast<IMyGameInterface>((UObject*)GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		AMyGameMode* MyGameMode = Cast<AMyGameMode>(GameMode);
		if (MyGameMode)
		{
			MyGameMode->SetupHUDWidget(this);
		}
	}

	// ���2 : ���� ������Ʈ���� �ٷ� �ð� ������, ����/Ŭ���̾�Ʈ ��� ������Ʈ ��.
	/* �� ������� �ٲ� ��� MyGameInterface���� �����Լ� ����
	AMyGameState* GameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		UpdateTimer(GameState->RemainingTime);
	}
	*/
}

void UMyMatchTimerWidget::UpdateTimer(float NewTime)
{
	CurrentTime = NewTime;

	ensure(CurrentTime >= 0);

	if (Timer)
	{
		Timer->SetText(FText::FromString(GetTimerText()));
	}
}

FString UMyMatchTimerWidget::GetTimerText()
{
	return FString::Printf(TEXT("%.0f"), CurrentTime);
}
