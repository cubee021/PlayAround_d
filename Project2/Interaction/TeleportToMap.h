// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportToMap.generated.h"

UCLASS()
class PROJECT2_API ATeleportToMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportToMap();

protected:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UBoxComponent> Trigger;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	/** Name of a level where player will travel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TravelTo)
		FString MapName;
	/** Game mode of MapName */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TravelTo)
		FString GameModePath;

};
