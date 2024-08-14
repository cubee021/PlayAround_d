// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FMyCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMyCharacterStat() : MaxHp(0.0f), Attack(0.0f), AttackRange(0.0f), AttackSpeed(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
		float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
		float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
		float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
		float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
		float MovementSpeed;

	FMyCharacterStat operator+(const FMyCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FMyCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FMyCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		uint32 uMaxHp = (uint32)MaxHp;
		Ar.SerializeIntPacked(uMaxHp);
		MaxHp = (float)uMaxHp;

		uint32 uAttack = (uint32)Attack;
		Ar.SerializeIntPacked(uAttack);
		Attack = (float)uAttack;

		uint32 uAttackRange = (uint32)AttackRange;
		Ar.SerializeIntPacked(uAttackRange);
		AttackRange = (float)uAttackRange;

		uint32 uAttackSpeed = (uint32)AttackSpeed;
		Ar.SerializeIntPacked(uAttackSpeed);
		AttackSpeed = (float)uAttackSpeed;

		uint32 uMovementSpeed = (uint32)MovementSpeed;
		Ar.SerializeIntPacked(uMovementSpeed);
		MovementSpeed = (float)uMovementSpeed;

		return true;
	}

};

template<>
struct TStructOpsTypeTraits<FMyCharacterStat> : public TStructOpsTypeTraitsBase2<FMyCharacterStat>
{
	enum
	{
		WithNetSerializer = true
	};
};
