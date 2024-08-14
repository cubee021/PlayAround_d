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

	// 방법1 : 게임모드 거쳐서 시간 업데이트, 클라이언트는 업데이트 안됨.
	IMyGameInterface* GameMode = Cast<IMyGameInterface>((UObject*)GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		AMyGameMode* MyGameMode = Cast<AMyGameMode>(GameMode);
		if (MyGameMode)
		{
			MyGameMode->SetupHUDWidget(this);
		}
	}

	// 방법2 : 게임 스테이트에서 바로 시간 가져옴, 서버/클라이언트 모두 업데이트 됨.
	/* 이 방법으로 바꿀 경우 MyGameInterface에서 가상함수 삭제
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
