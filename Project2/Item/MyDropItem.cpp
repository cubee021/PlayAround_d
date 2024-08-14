// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MyDropItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/MyCharacterBase.h"
#include "MyItemData.h"
#include "MyMeleeItemData.h"
#include "MyGunItemData.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyDropItem::AMyDropItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	MeleeSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeleeSkeletal"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	RootComponent = Trigger;
	MeleeSkeletal->SetupAttachment(Trigger);

	Trigger->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	Trigger->SetBoxExtent(FVector(40.0f, 40.0f, 30.0f));
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	MeleeSkeletal->SetCollisionProfileName(TEXT("NoCollision"));
	MeleeSkeletal->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	MeleeSkeletal->SetRelativeRotation(FRotator(45.f, 0.f, 0.f));

	ProjectileMovementComponent->SetUpdatedComponent(Trigger);
	ProjectileMovementComponent->InitialSpeed = 300.0f;
	ProjectileMovementComponent->MaxSpeed = 300.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = false;

	bReplicates = true;
}

void AMyDropItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyDropItem::OnOverlapBegin);
}

// Called every frame
void AMyDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MeleeSkeletal->AddRelativeRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));

	Duration -= DeltaTime;

	if (Duration < 0.f)
	{
		Destroy();
	}

	if (Duration < 2.f)
	{
		if (FlickeringTerm < 0)
		{
			if (MeleeSkeletal->IsVisible())
			{
				MeleeSkeletal->SetVisibility(false);
			}
			else
			{
				MeleeSkeletal->SetVisibility(true);
			}

			FlickeringTerm = 0.2f;
		}

		FlickeringTerm -= DeltaTime;
	}
}

void AMyDropItem::SetItemToDrop(UMyItemData* InItem)
{
	Item = InItem;
	ensure(Item);

	SetItemSkeletal();
}

void AMyDropItem::SetItemSkeletal()
{
	UMyMeleeItemData* MeleeItem = Cast<UMyMeleeItemData>(Item);
	if (MeleeItem)
	{
		if (MeleeItem->WeaponMesh.IsPending())
		{
			MeleeItem->WeaponMesh.LoadSynchronous();
		}
		MeleeSkeletal->SetSkeletalMesh(MeleeItem->WeaponMesh.Get());

		return;
	}

	UMyGunItemData* GunItem = Cast<UMyGunItemData>(Item);
	if (GunItem)
	{
		if (GunItem->WeaponMesh.IsPending())
		{
			GunItem->WeaponMesh.LoadSynchronous();
		}
		MeleeSkeletal->SetSkeletalMesh(GunItem->WeaponMesh.Get());
	}
}

bool AMyDropItem::FireInDirection(const FVector& ShootDirection)
{
	if (Item == nullptr || !MeleeSkeletal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error : Drop item failed!"));
		return 0;
	}

	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;

	return 1;
}

void AMyDropItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	AMyCharacterBase* Character = Cast<AMyCharacterBase>(OtherActor);
	if (Character)
	{

	}
}

