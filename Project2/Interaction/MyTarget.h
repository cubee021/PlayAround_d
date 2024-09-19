// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyTarget.generated.h"

/**
* 플레이어가 MyBullet으로 맞추면 Play point를 얻는 과녁
*/
UCLASS()
class PROJECT2_API AMyTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
		TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		TObjectPtr<class UStaticMeshComponent> Mesh;

	/** Explosion effect when this actor is destroyed */
	UPROPERTY(VisibleAnywhere, Category = Effect)
		TObjectPtr<class UParticleSystemComponent> Effect;

	/** Mesh becomes invisible when overlapped */
	void TargetMeshInvisible();

	FTimerHandle InvisibleTimerhandle;
	/** How long this actor will be invisible before show up */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Duration)
		float InvisibleDuration = 4.f;

protected:
	/** Damage count this actor gets before destroyed */
	int32 DamageCount = 0;
	/** When DamageCount == MaxDamageCount, this actor becomes invisible */
	int32 MaxDamageCount = 3;

public:
	/** If true, this target is attackable */
	UPROPERTY(ReplicatedUsing = OnRep_HasDamagedMax)
		uint8 bAttackable : 1;
	/** Play point player will get per attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackPoint)
		int32 AttackPoint = 1;

public:
	/**
	* Server Section */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
		void OnRep_HasDamagedMax();

};
