// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionRoute.h"
#include "Components/SplineComponent.h"

// Sets default values
AInteractionRoute::AInteractionRoute()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

	SplineComponent->SetupAttachment(RootComponent);
	SplineComponent->Duration = 10.f;
	SplineComponent->bDrawDebug = true;

	static ConstructorHelpers::FClassFinder<AActor> ActorToMoveRef(TEXT("/Script/Engine.Blueprint'/Game/Project2/Blueprint/TargetMesh/BP_MyTarget1.BP_MyTarget1_C'"));
	if (ActorToMoveRef.Class)
	{
		ActorToMoveClass = ActorToMoveRef.Class;
	}

	bSplineInLoop = true;
	TargetSize = FVector(1.f, 1.f, 1.f);
}

// Called when the game starts or when spawned
void AInteractionRoute::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		ActorToMove = GetWorld()->SpawnActor<AActor>(ActorToMoveClass, SplineComponent->GetComponentTransform());
		if (ActorToMove)
		{
			ActorToMove->SetActorScale3D(TargetSize);
			StartTime = GetWorld()->GetTimeSeconds();
		}

	}
}

// Called every frame
void AInteractionRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActorToMove)
	{
		float CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / SplineComponent->Duration;
		float Distance = SplineComponent->GetSplineLength() * CurrentSplineTime;

		FVector Position = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		ActorToMove->SetActorLocation(Position);

		if (CurrentSplineTime >= 1.f)
		{
			if (bSplineInLoop)
			{
				StartTime = GetWorld()->GetTimeSeconds();
				CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / SplineComponent->Duration;
			}
		}
	}
}

