// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItemData.h"
#include "Project2/GameData/MyCharacterStat.h"
#include "MyGunItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyGunItemData : public UMyItemData
{
	GENERATED_BODY()
	
public:
	UMyGunItemData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MyItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Weapon)
		TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
		FMyCharacterStat ModifierStat;
};
