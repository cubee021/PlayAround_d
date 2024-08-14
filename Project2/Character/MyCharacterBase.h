// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/MyCharacterItemInterface.h"
#include "Interface/MyCharacterWidgetInterface.h"
#include "Interface/MyAttackHitInterface.h"
#include "GameData/MyCharacterStat.h"
#include "Item/MyItemData.h"
#include "MyComboAttackData.h"
#include "MyCharacterBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UMyItemData* /*InItemData*/)

/** Player's perspective during the gameplay */
UENUM()
enum class ECharacterControlType : uint8
{
	ThirdPerson,
	FirstPerson
};

/** Delegate wrapper 사용으로 네트워크를 위한 상태 공유 시 직렬화 가능 */
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {};
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}

	FOnTakeItemDelegate ItemDelegate;
};

USTRUCT(BlueprintType)
struct FComboAttackWrapper
{
	GENERATED_BODY()
	FComboAttackWrapper() {};
	FComboAttackWrapper(UAnimMontage* InComboMontage, UMyComboAttackData* InComboAttackData)
		: ComboMontage(InComboMontage), ComboAttackData(InComboAttackData) {}

	TObjectPtr<UAnimMontage> ComboMontage;
	TObjectPtr<UMyComboAttackData> ComboAttackData;
};

/** Base of all MyCharacters generated in World.
* It manages overall functions of player and non-player have in common such as :
* mesh, attack actions, damage, apply item, dead */
UCLASS()
class PROJECT2_API AMyCharacterBase : public ACharacter, public IMyCharacterItemInterface, public IMyCharacterWidgetInterface, public IMyAttackHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterBase();

	virtual void PostInitializeComponents() override;

protected:
	virtual void SetCharacterControlData(const class UMyCharacterControlData* CharacterControlData);

	/** Binds character control data asset with enum */
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
		TMap<ECharacterControlType, class UMyCharacterControlData*> CharacterControlManager;


protected:
	/**
	* Combo punch, combo melee attack section */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TObjectPtr<class UAnimMontage> ComboPunchMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TObjectPtr<class UAnimMontage> ComboMeleeMontage;

	/** Info about combo name prefix and count of ComboPunchMontage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UMyComboAttackData> ComboPunchData;

	/** Info about combo name prefix and count of ComboMeleeMontage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UMyComboAttackData> ComboMeleeData;

	UPROPERTY()
	TArray<FComboAttackWrapper> ComboAttackManager;

	void ProcessComboCommand();

	void ComboAttackBegin();
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	/** Delay time between current combo action and next combo action */
	void SetComboCheckTimer();
	/** Check if next combo action remaining */
	void ComboCheck();

	/** Current index of Montage section */
	int32 CurrentCombo = 0;

	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

protected:
	virtual void AttackHitCheck() override;

protected:
	/**
	* Damage, dead section */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAnimMontage> DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAnimMontage> DeadMontage;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetDamaged(float CurrentHp, float MaxHp);
	virtual void SetDead();

	void PlayDamageAnimation();
	void PlayDeadAnimation();

	/** Delay between dead action and idle(get up) */
	float DeadEventDelayTime = 5.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UMyCharacterStatComponent> Stat;

protected:
	/** UI widget section
	* This sets Hp bar on top of each MyCharacter's head */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class UMyUserWidget* InUserwidget) override;

public:
	/** 
	Weapon section */
	EWeaponType GetCurrentWeaponType();
	void SetCurrentWeaponType(UMyItemData* InMyItemData);

	EWeaponType CurrentWeaponType;

protected:
	/** Place where weapon mesh will be set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class USkeletalMeshComponent> WeaponSocket;

	/**
	* 생성자에서 weapon type에 맞게 FTakeItemDelegateWrapper를 등록.
	* 무기를 획득하면 TakeItem()에 입력된 InItemData에 따라 실행할 함수를 각각 다르게 호출할 수 있음
	*/
	UPROPERTY()
		TArray<FTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UMyItemData* InItemData) override;

	/** Set Stat and WeaponSocket as following ItemData*/
	virtual void EquipMelee(class UMyItemData* InItemData);
	virtual void EquipGun(class UMyItemData* InItemData);

	/** Default Stat(Punch) 
	* Use this to return to punch from holding melee or gun */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UMyItemData> NoWeaponData;

	virtual void DetachWeapon(UMyItemData* InItemData);

	/** Check if player is holding a weapon on hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		uint8 bIsHoldingWeapon : 1;
	/** 보유한 무기 -> 빈손(punch) -> 무기 toggle 시 보유한 무기 정보 저장 */
	class UMyItemData* SavedItemData;

public:
	void TakeHealItem(float InHealAmount);

protected:
	/**
	* Gun shoot section */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAnimMontage> ShootMontage;

	void PlayShootAnimation();

public:
	/**
	Level Stat section (이건 그냥 강의 따라서 만들어본거)
	*/
	int32 GetLevel();
	void SetLevel(int32 InNewLevel);
	void ApplyStat(const FMyCharacterStat& BaseStat, const FMyCharacterStat& ModifierStat);

};
