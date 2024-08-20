// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MyDropItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/MyCharacterBase.h"
#include "Interface/MyCharacterItemInterface.h"
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
	ItemSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletal"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	RootComponent = Trigger;
	ItemSkeletal->SetupAttachment(Trigger);
	ItemSkeletal->SetIsReplicated(true); // 소용 없음

	Trigger->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	Trigger->SetBoxExtent(FVector(40.0f, 40.0f, 30.0f));
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	ItemSkeletal->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSkeletal->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	ItemSkeletal->SetRelativeRotation(FRotator(45.f, 0.f, 0.f));


	ProjectileMovementComponent->SetUpdatedComponent(Trigger);
	ProjectileMovementComponent->InitialSpeed = 300.0f;
	ProjectileMovementComponent->MaxSpeed = 300.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = false;

	bReplicates = true;
	SetReplicateMovement(true);
	FlickeringTimer = FlickeringTerm;
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

	ItemSkeletal->AddRelativeRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));

	Duration -= DeltaTime;

	if (Duration < 0.f)
	{
		Destroy();
	}

	if (Duration < 2.f)
	{
		if (FlickeringTimer < 0)
		{
			if (ItemSkeletal->IsVisible())
			{
				ItemSkeletal->SetVisibility(false);
			}
			else
			{
				ItemSkeletal->SetVisibility(true);
			}

			FlickeringTimer = FlickeringTerm;
		}

		FlickeringTimer -= DeltaTime;
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
		ItemSkeletal->SetSkeletalMesh(MeleeItem->WeaponMesh.Get());

		return;
	}

	UMyGunItemData* GunItem = Cast<UMyGunItemData>(Item);
	if (GunItem)
	{
		if (GunItem->WeaponMesh.IsPending())
		{
			GunItem->WeaponMesh.LoadSynchronous();
		}
		ItemSkeletal->SetSkeletalMesh(GunItem->WeaponMesh.Get());
	}
}

void AMyDropItem::OnRep_SetItemToDrop()
{
	ensure(Item);
	SetItemSkeletal();
}

bool AMyDropItem::FireInDirection(const FVector& ShootDirection)
{
	if (Item == nullptr || !ItemSkeletal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error : Drop item failed!"));
		return 0;
	}

	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;

	return 1;
}

void AMyDropItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	IMyCharacterItemInterface* Character = Cast<IMyCharacterItemInterface>(OtherActor);
	if (Character)
	{
		Character->TakeItem(Item);
		Destroy();
	}
}

void AMyDropItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyDropItem, Item);
}

