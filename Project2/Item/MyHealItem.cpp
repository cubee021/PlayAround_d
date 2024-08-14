// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHealItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Project2/Character/MyCharacterBase.h"
#include "Project2/Physics//MyCollision.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyHealItem::AMyHealItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_MYTRIGGER);
	Trigger->SetBoxExtent(FVector(35.f, 12.f, 35.f));
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Prototype_Project/Meshes/Icons/SM_Health_Icon_01_1.SM_Health_Icon_01_1'"));
	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	bReplicates = true;
	bOverlapped = false;
}

void AMyHealItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HealAmount = FMath::RandRange(10, 100);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyHealItem::OnOverlapBegin);
}

// Called every frame
void AMyHealItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
}

void AMyHealItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	AMyCharacterBase* Character = Cast<AMyCharacterBase>(OtherActor);
	if (Character)
	{
		Character->TakeHealItem(HealAmount);
	}

	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	bOverlapped = true;
}

void AMyHealItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyHealItem, bOverlapped);
}

void AMyHealItem::OnRep_HasOverlapped()
{
	if (bOverlapped)
	{
		Mesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

