// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyCharacterStat.h"
#include "MyGameSingleton.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyGameSingleton : public UObject
{
	GENERATED_BODY()

public:

	UMyGameSingleton();
	static UMyGameSingleton& Get();

public:
	FORCEINLINE FMyCharacterStat GetCharacterStat(int32 InLevel) const 
	{
		return CharacterStatTable.IsValidIndex(InLevel - 1) ? CharacterStatTable[InLevel - 1] : FMyCharacterStat();
	}

	UPROPERTY()
		int32 CharacterMaxLevel;

private:
	TArray<FMyCharacterStat> CharacterStatTable;
};
