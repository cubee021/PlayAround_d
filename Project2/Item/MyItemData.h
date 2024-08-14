// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyItemData.generated.h"

/**
 * Weapon type only. Use other Data Asset class to use heal function
 */

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Melee = 0,
	Gun,
	NoWeapon
};

UCLASS()
class PROJECT2_API UMyItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MyItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
		EWeaponType Type;
	
};
