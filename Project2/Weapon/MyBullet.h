// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBullet.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBulletHitDelegate, AActor* /*DamagedCharacter*/);

UCLASS()
class PROJECT2_API AMyBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyBullet();

protected:
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Same as a trigger */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TObjectPtr<class USphereComponent> CollisionComponent;
	/** This will manage bullet as a projectile */
	UPROPERTY(VisibleAnywhere, Category = Movement)
		TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;
	/** Move this projectile in a shoot direction with default speed */
	void FireInDirection(const FVector& ShootDirection, APawn* InShooter, float InDamageAmount);

protected:
	void BulletDestroy();

	FTimerHandle ExpireTimerHandle;
	/** How long this bullet will remain in World after spawned */
	float Duration = 10.f;

protected:
	/** Overlapped player will take damage */
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);
	/** When hit walls or floors in World, it will increase CollideCount.
	* When hit more than twice, set bCollidable to false, so this bullet can no longer damage player */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	/** If true, bullet can damage overlapped player */
	uint8 bCollidable : 1;
	/** Count how many times this bullet has hit walls or floors in World */
	int32 CollideCount;

public:
	/** Actor who spawned this bullet */
	APawn* Shooter;
protected:
	/** Damage amount overlapped player will get */
	float DamageAmount;

public:
	/** Broadcast with damaged player */
	FOnBulletHitDelegate OnBulletHit;

};
