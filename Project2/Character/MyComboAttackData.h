// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyComboAttackData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyComboAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyComboAttackData();
	
	UPROPERTY(EditAnywhere, Category = Name)
		FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = Name)
		uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = Name)
		float FrameRate;

	UPROPERTY(EditAnywhere, Category = ComboData)
		TArray<float> EffectiveFrameCount;
};
