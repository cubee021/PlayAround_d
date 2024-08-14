// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDropItem.generated.h"

UCLASS()
class PROJECT2_API AMyDropItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyDropItem();

protected:
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Rotation rate of skeletal mesh */
	float RotationRate = 50.f;

	float Duration = 5.f;
	float FlickeringTerm = 0.2f;

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
		TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(EditAnywhere, Category = Item)
		TObjectPtr<class UMyItemData> Item;

	UPROPERTY(VisibleAnywhere, Category = Item)
		TObjectPtr<class USkeletalMeshComponent> MeleeSkeletal;
	/** Manage this Actor as a projectile */
	UPROPERTY(VisibleAnywhere, Category = Movement)
		TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	void SetItemSkeletal();

public:
	/** Move this projectile in a shoot direction with default speed */
	bool FireInDirection(const FVector& ShootDirection);
	void SetItemToDrop(class UMyItemData* InItem);

protected:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

};
