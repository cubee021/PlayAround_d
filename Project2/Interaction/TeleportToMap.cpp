// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/TeleportToMap.h"
#include "Components/BoxComponent.h"
#include "Physics//MyCollision.h"
#include "Character/MyCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATeleportToMap::ATeleportToMap()
{
 	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));

	RootComponent = Trigger;

	Trigger->SetCollisionProfileName(CPROFILE_MYTRIGGER);

	GameModePath = FString(TEXT("/Script/Project2.MyGameMode"));
}

void ATeleportToMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATeleportToMap::OnOverlapBegin);
}

void ATeleportToMap::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	AMyCharacterBase* Character = Cast<AMyCharacterBase>(OtherActor);
	if (Character)
	{
		FString URL = MapName + "?game=" + GameModePath + "?listen";
		GetWorld()->ServerTravel(URL); // relative travel that no actors become initialized
	}
}
