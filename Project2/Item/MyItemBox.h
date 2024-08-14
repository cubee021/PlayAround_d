// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItemBox.generated.h"

UCLASS()
class PROJECT2_API AMyItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItemBox();

	FORCEINLINE class UBoxComponent* GetTrigger() { return Trigger; }

protected:
	virtual void PostInitializeComponents() override;

public:
	virtual void Tick(float DeltaSeconds);

	/** Rotation rate of weapon skeletal mesh */
	float RotationRate = 50.f;

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
		TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
		TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = Item)
		TObjectPtr<class UMyItemData> Item;

	/** When Item box trigger and player overlap, attach random weapon */
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

protected:
	/** Show player what weapon is currently in item box */
	UPROPERTY(VisibleAnywhere, Category = Item)
		TObjectPtr<class USkeletalMeshComponent> MeleeSkeletal;

	/** Set MeleeSkeletal same as a weapon data */
	void SetItemMesh();

public:
	FTimerHandle ItemRespawnTimerHandle;
	/** Term between next Item to be respawned from current item obtained */
	int32 RespawnTime = 3.f;
public:
	/**
	* Server section */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Toggle Itembox's current state */
	UFUNCTION()
	void OnRep_SetItemMeshOnOff();
	/** Set random item(melee, gun) in item box */
	void SetRandomItemData();
	/** Server notify everyone item is newly set and okay to obtain */
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRPCSetItem(class UMyItemData* NewItemData);
	/** Client notify Server that is item is taken by client. Server will set bItemObtainable to false */
	UFUNCTION(Server, Unreliable)
		void ServerRPCItemTakenByClient();

	/** If true, itembox is in a state player can get item from it */
	UPROPERTY(ReplicatedUsing = OnRep_SetItemMeshOnOff)
		uint8 bItemObtainable : 1;
};
