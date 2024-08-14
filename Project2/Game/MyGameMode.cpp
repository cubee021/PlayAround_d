// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameSession.h"
#include "Project2/Player/MyPlayerController.h"
#include "Project2/UI/MyMatchTimerWidget.h"
#include "MyGameState.h"
#include "MyPlayerState.h"
#include "EngineUtils.h"
#include "Character/MyCharacterBase.h"

AMyGameMode::AMyGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/Project2/Blueprint/BP_MyCharacterPlayer.BP_MyCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	PlayerControllerClass = AMyPlayerController::StaticClass();
	GameStateClass = AMyGameState::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();

	bUseSeamlessTravel = true;
}

void AMyGameMode::StartPlay()
{
	Super::StartPlay();

	for (APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		PlayerStartArray.Add(PlayerStart);
	}
}

void AMyGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AMyGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AMyGameMode::DefaultGameTimer()
{
	AMyGameState* const MyGameState = Cast<AMyGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0)
	{
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();
			}
		}
	}

}

void AMyGameMode::FinishMatch()
{
	AMyGameState* const MyGameState = Cast<AMyGameState>(GameState);
	if (MyGameState && IsMatchInProgress())
	{
		EndMatch();

		OnEndMatchDelegate.Broadcast();
	}
}

void AMyGameMode::SetupHUDWidget(UMyMatchTimerWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		AMyGameState* const MyGameState = Cast<AMyGameState>(GameState);
		if (MyGameState)
		{
			InHUDWidget->UpdateTimer(MyGameState->RemainingTime);
		}
	}
}

FTransform AMyGameMode::GetRandomStartTransform() const
{
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.f, 0.f, 230.f));
	}

	int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);

	return PlayerStartArray[RandIndex]->GetActorTransform();
}

void AMyGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	AMyPlayerState* KillerPlayerState = Cast<AMyPlayerState>(Killer->PlayerState);
	if (KillerPlayerState)
	{
		KillerPlayerState->SetKillPoint(KillerPlayerState->GetKillPoint() + 1);

		AMyCharacterBase* Character = Cast<AMyCharacterBase>(Killer->GetPawn());
		if (Character)
		{
			int LevelUpInterval = 2;

			if (KillerPlayerState->GetKillPoint() % LevelUpInterval == 0)
			{
				Character->SetLevel(Character->GetLevel() + 1);
			}
		}
	}
}
/*
void AMyGameMode::PostSeamlessTravel()
{
	if (GameSession)
	{
		GameSession->PostSeamlessTravel();
	}

	// We have to make a copy of the controller list, since the code after this will destroy
	// and create new controllers in the world's list
	TArray<AController*> OldControllerList;
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		OldControllerList.Add(It->Get());

		APlayerController* PC = Cast<APlayerController>(It->Get());
		if (PC->GetPawn() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Success"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed"));
		}
	}

	// handle players that are already loaded
	for (AController* Controller : OldControllerList)
	{
		if (Controller->PlayerState)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController == nullptr)
			{
				HandleSeamlessTravelPlayer(Controller);
			}
			else
			{
				if (MustSpectate(PlayerController))
				{
					// The spectator count must be incremented here, instead of in HandleSeamlessTravelPlayer,
					// as otherwise spectators can 'hide' from player counters, by making HasClientLoadedCurrentWorld return false
					NumSpectators++;
				}
				else
				{
					NumTravellingPlayers++;
				}
				if (PlayerController->HasClientLoadedCurrentWorld())
				{
					HandleSeamlessTravelPlayer(Controller);
				}
			}
		}
	}
}

void AMyGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// If players should start as spectators, leave them in the spectator state
	if (!bStartPlayersAsSpectators && !MustSpectate(NewPlayer))
	{
		// If match is in progress, start the player
		if (IsMatchInProgress() && PlayerCanRestart(NewPlayer))
		{
			if (ControllerPawnToCopy != nullptr)
			{
				NewPlayer->SetPawn(ControllerPawnToCopy);

				UE_LOG(LogTemp, Warning, TEXT("Ishere! : Implement pawn successful"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Ishere! : Implement pawn error.. nullptr"));
			}

			RestartPlayer(NewPlayer);
		}
		// Check to see if we should start right away, avoids a one frame lag in single player games
		else if (GetMatchState() == MatchState::WaitingToStart)
		{
			// Check to see if we should start the match
			if (ReadyToStartMatch())
			{
				StartMatch();
			}
		}
	}
}
*/

/*
void AMyGameMode::SetupHUDWidget(UMyMatchTimerWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		AMyGameState* const MyGameState = Cast<AMyGameState>(GameState);
		if (MyGameState)
		{
			InHUDWidget->UpdateTimer(MyGameState->RemainingTime);
		}
	}
}
*/
