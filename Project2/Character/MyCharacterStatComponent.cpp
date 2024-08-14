// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Project2/GameData/MyGameSingleton.h"

// Sets default values for this component's properties
UMyCharacterStatComponent::UMyCharacterStatComponent()
{
	CurrentLevel = 1;
	AttackRadius = 50.f;

	bWantsInitializeComponent = true;
}

void UMyCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	ResetStat();

	OnStatChanged.AddUObject(this, &UMyCharacterStatComponent::SetNewMaxHp);
	SetIsReplicated(true);
}

void UMyCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UMyGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UMyGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0.0f);
}

float UMyCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.f, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UMyCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, MaxHp);

	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UMyCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyCharacterStatComponent, CurrentHp);
	DOREPLIFETIME(UMyCharacterStatComponent, MaxHp);
	DOREPLIFETIME_CONDITION(UMyCharacterStatComponent, BaseStat, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMyCharacterStatComponent, ModifierStat, COND_OwnerOnly);
}

void UMyCharacterStatComponent::SetNewMaxHp(const FMyCharacterStat& InBaseStat, const FMyCharacterStat& InModifierStat)
{
	float PrevMaxHp = MaxHp;
	MaxHp = GetTotalStat().MaxHp;
	if (PrevMaxHp != MaxHp)
	{
		OnHpChanged.Broadcast(CurrentHp, MaxHp);
	}
}

void UMyCharacterStatComponent::OnRep_CurrentHp()
{
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

void UMyCharacterStatComponent::OnRep_MaxHp()
{
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UMyCharacterStatComponent::OnRep_BaseStat()
{
	OnStatChanged.Broadcast(BaseStat, ModifierStat);
}

void UMyCharacterStatComponent::OnRep_ModifierStat()
{
	OnStatChanged.Broadcast(BaseStat, ModifierStat);
}

void UMyCharacterStatComponent::ResetStat()
{
	SetLevelStat(CurrentLevel);
	MaxHp = BaseStat.MaxHp;
	SetHp(MaxHp);
}
