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
	/** How long this will be last after drop */
	float Duration = 5.f;
	/** Mesh begins to flicker before it is destroyed*/
	float FlickeringTerm = 0.2f;
	float FlickeringTimer;

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
		TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(ReplicatedUsing = OnRep_SetItemToDrop, EditAnywhere, Category = Item)
		TObjectPtr<class UMyItemData> Item;

	/** SkeletalMeshComponent of an item to set Skeletal mesh */
	UPROPERTY(VisibleAnywhere, Category = Item)
		TObjectPtr<class USkeletalMeshComponent> ItemSkeletal;
	/** Manage this Actor as a projectile */
	UPROPERTY(VisibleAnywhere, Category = Movement)
		TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	/** Set Skeletal mesh depends on Item data */
	void SetItemSkeletal();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

public:
	/** Move this projectile in a shoot direction with default speed. True if successfully fired */
	bool FireInDirection(const FVector& ShootDirection);
	/** Change Item data and skeletal mesh MyCharacter currently has */
	void SetItemToDrop(class UMyItemData* InItem);

protected:
	/** Server Section */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Notify clients to change Item data and skeletal mesh */
	UFUNCTION()
		void OnRep_SetItemToDrop();
};
