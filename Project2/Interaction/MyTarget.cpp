// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/MyTarget.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyTarget::AMyTarget()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	Trigger->BodyInstance.SetCollisionProfileName(TEXT("Target"));

	RootComponent = Mesh;
	Trigger->SetupAttachment(Mesh);
	Effect->SetupAttachment(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Prototype_Project/Meshes/Icons/Sm_Human_Obstacle_01_2.Sm_Human_Obstacle_01_2'"));
	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/Prototype_Project/ParticleSystem/PS_Explosion.PS_Explosion'"));
	if (EffectRef.Object)
	{
		Effect->SetTemplate(EffectRef.Object);
		Effect->bAutoActivate = false;
	}

	bAttackable = true;

}

// Called when the game starts or when spawned
void AMyTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyTarget::TargetMeshInvisible()
{
	if (DamageCount >= MaxDamageCount)
	{
		bAttackable = false;

		Effect->Activate(true);

		Mesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);

		GetWorldTimerManager().SetTimer(InvisibleTimerhandle, FTimerDelegate::CreateLambda([this]()
			{

				Mesh->SetHiddenInGame(false);
				SetActorEnableCollision(true);

				bAttackable = true;
				DamageCount = 0;

			}), InvisibleDuration, false);
	}
	else
	{
		DamageCount++;
	}
}

void AMyTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyTarget, bAttackable);
}

void AMyTarget::OnRep_HasDamagedMax()
{
	if (bAttackable)
	{
		Mesh->SetHiddenInGame(false);
		SetActorEnableCollision(true);
	}
	else
	{
		Mesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}



