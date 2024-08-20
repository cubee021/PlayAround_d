// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

AMyPlayerState::AMyPlayerState()
{
	KillPoint = 0;
	PlayPoint = 0;
}

void AMyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerState);
	if (MyPlayerState)
	{
		MyPlayerState->SetKillPoint(GetKillPoint());
		MyPlayerState->SetPlayPoint(GetPlayPoint());
	}
}


