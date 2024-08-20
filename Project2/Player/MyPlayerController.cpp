// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "UI/MyHUDWidget.h"
#include "UI/MyMatchEndWidget.h"
#include "Game/MyGameMode.h"

AMyPlayerController::AMyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UMyHUDWidget> MyHUDWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Project2/UI/WBP_CharacterHUD.WBP_CharacterHUD_C'"));
	if (MyHUDWidgetRef.Class)
	{
		MyHUDWidgetClass = MyHUDWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UMyMatchEndWidget> MyMatchEndWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Project2/UI/WBP_MatchEnd.WBP_MatchEnd_C'"));
	if (MyMatchEndWidgetRef.Class)
	{
		MyMatchEndWidgetClass = MyMatchEndWidgetRef.Class;
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		MyHUDWidget = CreateWidget(this, MyHUDWidgetClass);
		if (MyHUDWidget)
		{
			MyHUDWidget->AddToViewport();
		}

		MyMatchEndWidget = CreateWidget(this, MyMatchEndWidgetClass);
		if (MyMatchEndWidget)
		{
			MyMatchEndWidget->AddToViewport();
		}

		MyMatchEndWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	// GameMode�� ����only, GameState�� ����, Ŭ���̾�Ʈ
	// delegate�� ���� ���ؼ� client������ ��� ȭ���� ������ ����
	AMyGameMode* GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnEndMatchDelegate.AddUObject(this, &AMyPlayerController::ShowMatchEndWidget);
	}

	// Control directly turn to game input when play start
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void AMyPlayerController::ShowMatchEndWidget()
{
	if (IsLocalController())
	{
		UMyMatchEndWidget* MatchEndWidget = Cast<UMyMatchEndWidget>(MyMatchEndWidget);
		if (MatchEndWidget)
		{
			MatchEndWidget->SetFinalScore();
		}

		MyMatchEndWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

