// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/MyCollision.h"
#include "Interface/MyCharacterItemInterface.h"
#include "Engine/AssetManager.h"
#include "MyItemData.h"
#include "MyMeleeItemData.h"
#include "MyGunItemData.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyItemBox::AMyItemBox()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeleeSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeleeSkeletal"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	MeleeSkeletal->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_MYTRIGGER);
	Trigger->SetBoxExtent(FVector(40.0f, 40.0f, 30.0f));
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Prototype_Project/Meshes/Props/SM_Pipe_1x1_1.SM_Pipe_1x1_1'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	Mesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -180.0f));
	//Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	MeleeSkeletal->SetCollisionProfileName(TEXT("NoCollision"));
	MeleeSkeletal->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	MeleeSkeletal->SetRelativeRotation(FRotator(45.f, 0.f, 0.f));

	bReplicates = true;
	bItemObtainable = true;
}

void AMyItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 초기 설정된 weapon이 없으면 랜덤으로 지정
	if (!::IsValid(Item))
	{
		SetRandomItemData();
	}
	ensure(Item);

	SetItemMesh();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyItemBox::OnOverlapBegin);
}

void AMyItemBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MeleeSkeletal->AddRelativeRotation(FRotator(0.f, RotationRate * DeltaSeconds, 0.f));

}

void AMyItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (nullptr == Item)
	{
		Destroy();
		return;
	}

	IMyCharacterItemInterface* OverlappingPawn = Cast<IMyCharacterItemInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Item);
	}

	//Mesh->SetHiddenInGame(true);
	MeleeSkeletal->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	bItemObtainable = false;

	if (!HasAuthority())
	{
		SetOwner(GetWorld()->GetFirstPlayerController());

		ServerRPCItemTakenByClient();
	}
	else
	{
		GetWorldTimerManager().SetTimer(ItemRespawnTimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				SetRandomItemData();

				MeleeSkeletal->SetHiddenInGame(false);
				SetActorEnableCollision(true);
				bItemObtainable = true;

				SetItemMesh();

				MulticastRPCSetItem(Item);

			}), RespawnTime, false);
	}
		
}

void AMyItemBox::SetItemMesh()
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

void AMyItemBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyItemBox, bItemObtainable);
}

void AMyItemBox::OnRep_SetItemMeshOnOff()
{
	if (bItemObtainable)
	{
		MeleeSkeletal->SetHiddenInGame(false);
		SetActorEnableCollision(true);
	}
	else
	{
		MeleeSkeletal->SetHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AMyItemBox::SetRandomItemData()
{
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("MyItemData"), Assets);
	ensure(0 < Assets.Num());

	int32 RandomIndex = FMath::RandRange(0, Assets.Num() - 1);

	FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets[RandomIndex]));
	if (AssetPtr.IsPending())
	{
		AssetPtr.LoadSynchronous();
	}
	Item = Cast<UMyItemData>(AssetPtr.Get());
}

void AMyItemBox::MulticastRPCSetItem_Implementation(class UMyItemData* NewItemData)
{
	Item = NewItemData;

	MeleeSkeletal->SetHiddenInGame(false);
	SetActorEnableCollision(true);
	bItemObtainable = true;

	SetItemMesh();
}

void AMyItemBox::ServerRPCItemTakenByClient_Implementation()
{
	GetWorldTimerManager().SetTimer(ItemRespawnTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			SetRandomItemData();

			MeleeSkeletal->SetHiddenInGame(false);
			SetActorEnableCollision(true);
			bItemObtainable = true;

			SetItemMesh();

			MulticastRPCSetItem(Item);

		}), RespawnTime, false);
}


