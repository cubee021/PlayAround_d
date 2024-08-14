// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project2/GameData/MyCharacterStat.h"
#include "MyCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChangeDelegate, float /*CurrentHp*/, float /*MaxHp*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangeDelegate, const FMyCharacterStat& /*BaseStat*/, const FMyCharacterStat& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT2_API UMyCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyCharacterStatComponent();

protected:
	virtual void InitializeComponent() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangeDelegate OnHpChanged;
	FOnStatChangeDelegate OnStatChanged;
		
	void SetLevelStat(int32 InNewLevel);
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }
	FORCEINLINE void AddBaseStat(const FMyCharacterStat& InAddBaseStat)
	{ 
		BaseStat = BaseStat + InAddBaseStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}
	FORCEINLINE void SetBaseStat(const FMyCharacterStat& InBaseStat)
	{
		BaseStat = InBaseStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}
	FORCEINLINE void SetModifierStat(const FMyCharacterStat& InModifierStat) 
	{ 
		ModifierStat = InModifierStat; 
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat()); 
	}

	FORCEINLINE const FMyCharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FMyCharacterStat& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE FMyCharacterStat GetTotalStat() const { return BaseStat + ModifierStat; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE void HealHp(float InHealAmount)
	{
		CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0.f, GetTotalStat().MaxHp);
		OnHpChanged.Broadcast(CurrentHp, MaxHp);
	}
	FORCEINLINE float GetAttackRadius() { return AttackRadius; }
	float ApplyDamage(float InDamage);


protected:
	void SetHp(float NewHp);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHp, Transient, VisibleInstanceOnly, Category = Stat)
		float CurrentHp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHp, Transient, VisibleInstanceOnly, Category = Stat)
		float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
		float CurrentLevel;

	UPROPERTY(VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		float AttackRadius;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BaseStat, VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		FMyCharacterStat BaseStat;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ModifierStat, VisibleInstanceOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
		FMyCharacterStat ModifierStat;


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetNewMaxHp(const FMyCharacterStat& InBaseStat, const FMyCharacterStat& InModifierStat);

	UFUNCTION()
		void OnRep_CurrentHp();

	UFUNCTION()
		void OnRep_MaxHp();

	UFUNCTION()
		void OnRep_BaseStat();

	UFUNCTION()
		void OnRep_ModifierStat();

public:
	void ResetStat();
};
