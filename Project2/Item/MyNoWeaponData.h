// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItemData.h"
#include "Project2/GameData/MyCharacterStat.h"
#include "MyNoWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyNoWeaponData : public UMyItemData
{
	GENERATED_BODY()

public:
	UMyNoWeaponData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("NoItemData", GetFName());
	}

public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
		FMyCharacterStat ModifierStat;
	
};
