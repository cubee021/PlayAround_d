// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyHealItem.generated.h"

UCLASS()
class PROJECT2_API AMyHealItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyHealItem();

protected:
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
		TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		TObjectPtr<class UStaticMeshComponent> Mesh;
	/** Random value of how mush heal item will heal overlapped player */
	UPROPERTY(EditAnywhere, Category = Hp)
		float HealAmount;
	/** Rotation rate of a mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	float RotationRate = 50.f;

	/** This will heal overlapped player and hide heal item */
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

public:
	/**
	* Server Section */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** If bOverlapped is true, it will turn off mesh and trigger so that player won't interact */
	UFUNCTION()
		void OnRep_HasOverlapped();

	UPROPERTY(ReplicatedUsing = OnRep_HasOverlapped)
		uint8 bOverlapped : 1;

};
