// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMatchEndWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextRenderComponent.h"
#include "Project2/Game/MyPlayerState.h"
#include "Project2/Game/MyGameMode.h"
#include "Project2/Player/MyPlayerController.h"
#include "EngineUtils.h"

UMyMatchEndWidget::UMyMatchEndWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMyMatchEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GridSlot = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("GridPanelResults")));
	ensure(GridSlot);

}

void UMyMatchEndWidget::SetFinalScore()
{
	int32 RowIndex = 1;

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController)
		{
			AMyPlayerState* PlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
			if (PlayerState)
			{
				UTextBlock* PlayerId = NewObject<UTextBlock>();
				UTextBlock* KillPoint = NewObject<UTextBlock>();
				UTextBlock* PlayPoint = NewObject<UTextBlock>();

				const FString PlayerIdStr = FString::Printf(TEXT("Player1"));
				PlayerId->SetText(FText::FromString(PlayerIdStr));

				const FString KillScoreStr = FString::Printf(TEXT("%d"), PlayerState->GetKillPoint());
				KillPoint->SetText(FText::FromString(KillScoreStr));

				const FString PlayScoreStr = FString::Printf(TEXT("%d"), PlayerState->GetPlayPoint());
				PlayPoint->SetText(FText::FromString(PlayScoreStr));

				GridSlot->AddChildToUniformGrid(PlayerId, RowIndex, 0);
				GridSlot->AddChildToUniformGrid(KillPoint, RowIndex, 1);
				GridSlot->AddChildToUniformGrid(PlayPoint, RowIndex, 2);
			}

			RowIndex++;
		}
	}
}
