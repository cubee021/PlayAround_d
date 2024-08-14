// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterControlData.h"
#include "Project2/Item/MyMeleeItemData.h"
#include "Project2/Item/MyGunItemData.h"
#include "Project2/Item/MyNoWeaponData.h"
#include "Project2/UI/MyWidgetComponent.h"
#include "Project2/UI/MyHpBarWidget.h"
#include "MyCharacterStatComponent.h"
#include "Project2/Physics/MyCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values
AMyCharacterBase::AMyCharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// Blueprint에서 따로 설정해서 주석 처리
	//GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_MYCAPSULE);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Toony_Tiny_Soldiers/Meshes/Characters_Prebuilt/SK_Soldier_16.SK_Soldier_16'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Project2/Animation/ABP_MyCharacter.ABP_MyCharacter_C'"));
	if(AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UMyCharacterControlData> FirstPersonDataRef(TEXT("/Script/Project2.MyCharacterControlData'/Game/Project2/CharacterControl/DA_FirstPerson.DA_FirstPerson'"));
	if (FirstPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::FirstPerson, FirstPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMyCharacterControlData> ThirdPersonDataRef(TEXT("/Script/Project2.MyCharacterControlData'/Game/Project2/CharacterControl/DA_ThirdPerson.DA_ThirdPerson'"));
	if (ThirdPersonDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::ThirdPerson, ThirdPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboPunchMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Project2/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboPunchMontageRef.Object)
	{
		ComboPunchMontage = ComboPunchMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboMeleeMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Project2/Animation/AM_ComboMelee.AM_ComboMelee'"));
	if (ComboMeleeMontageRef.Object)
	{
		ComboMeleeMontage = ComboMeleeMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMyComboAttackData> ComboPunchDataRef(TEXT("/Script/Project2.MyComboAttackData'/Game/Project2/CharacterAction/DA_ComboAttack.DA_ComboAttack'"));
	if (ComboPunchDataRef.Object)
	{
		ComboPunchData = ComboPunchDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMyComboAttackData> ComboMeleeDataRef(TEXT("/Script/Project2.MyComboAttackData'/Game/Project2/CharacterAction/DA_ComboMelee.DA_ComboMelee'"));
	if (ComboMeleeDataRef.Object)
	{
		ComboMeleeData = ComboMeleeDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Project2/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DamageMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Project2/Animation/AM_Damage.AM_Damage'"));
	if (DamageMontageRef.Object)
	{
		DamageMontage = DamageMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Project2/Animation/AM_GunShoot.AM_GunShoot'"));
	if (ShootMontageRef.Object)
	{
		ShootMontage = ShootMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMyItemData> NoWeaponDataRef(TEXT("/Script/Project2.MyNoWeaponData'/Game/Project2/Item/DA_NoWeapon.DA_NoWeapon'"));
	if (NoWeaponDataRef.Object)
	{
		NoWeaponData = NoWeaponDataRef.Object;
	}

	Stat = CreateDefaultSubobject<UMyCharacterStatComponent>(TEXT("Stat"));

	HpBar = CreateDefaultSubobject<UMyWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f,180.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarwidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Project2/UI/WBP_HpBar.WBP_HpBar_C'"));
	if (HpBarwidgetRef.Class)
	{
		// default settings of Hp bar  
		HpBar->SetWidgetClass(HpBarwidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.f, 15.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	/** weapon type에 맞게 함수 등록
	* 현재 상태 : 
	* TakeItemActions[0] : EWeaponType::Melee    -> EquipMelee()
	* TakeItemActions[1] : EWeaponType::Gun      -> EquipGun()
	* TakeItemActions[2] : EWeaponType::NoWeapon -> DetachWeapon()
	*/ 
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AMyCharacterBase::EquipMelee)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AMyCharacterBase::EquipGun)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AMyCharacterBase::DetachWeapon)));

	ComboAttackManager.Add(FComboAttackWrapper(ComboMeleeMontage, ComboMeleeData));
	ComboAttackManager.Add(FComboAttackWrapper(ComboPunchMontage, ComboPunchData)); // Gun은 아직 Montage 없음
	ComboAttackManager.Add(FComboAttackWrapper(ComboPunchMontage, ComboPunchData)); 

	WeaponSocket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponSocket->SetupAttachment(GetMesh(), TEXT("WeaponSocket_R"));

	CurrentWeaponType = EWeaponType::NoWeapon;
	SavedItemData = NoWeaponData;
	bIsHoldingWeapon = false;
}

void AMyCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &AMyCharacterBase::SetDead);
	Stat->OnHpChanged.AddUObject(this, &AMyCharacterBase::SetDamaged);
	Stat->OnStatChanged.AddUObject(this, &AMyCharacterBase::ApplyStat);
}

void AMyCharacterBase::SetCharacterControlData(const UMyCharacterControlData* CharacterControlData)
{
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AMyCharacterBase::ProcessComboCommand()
{
	if (CurrentCombo == 0)
	{
		if (GetCurrentWeaponType() == EWeaponType::Gun)
		{
			return;
		}

		ComboAttackBegin();
		return;
	}

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}

}

void AMyCharacterBase::ComboAttackBegin()
{
	CurrentCombo = 1;

	UAnimMontage* MontageToPlay = ComboAttackManager[(uint8)GetCurrentWeaponType()].ComboMontage;

	// Prevents stopping in the air during the attack montage
	if (!GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MontageToPlay, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AMyCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);

	ComboTimerHandle.Invalidate();

	SetComboCheckTimer();
}

void AMyCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AMyCharacterBase::SetComboCheckTimer()
{
	UMyComboAttackData* ComboAttackData = ComboAttackManager[(uint8)GetCurrentWeaponType()].ComboAttackData;

	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	float ComboEffectiveTime = (ComboAttackData->EffectiveFrameCount[ComboIndex] / ComboAttackData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AMyCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AMyCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();

	UAnimMontage* MontageToPlay = ComboAttackManager[(uint8)GetCurrentWeaponType()].ComboMontage;
	UMyComboAttackData* ComboAttackData = ComboAttackManager[(uint8)GetCurrentWeaponType()].ComboAttackData;

	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboAttackData->MaxComboCount);

		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboAttackData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, MontageToPlay);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AMyCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = Stat->GetTotalStat().AttackRange;
	const float AttackRadius = Stat->GetAttackRadius();
	const float AttackDamage = Stat->GetTotalStat().Attack;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_MYATTACKHIT, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.f);

#endif
}

void AMyCharacterBase::SetDamaged(float CurrentHp, float MaxHp)
{
	// CurrentHp == MaxHp인 경우 DeadMontage 이후 DamageMontage 재생되는 것 방지
	if (CurrentHp != MaxHp)
	{
		PlayDamageAnimation();
	}
}

float AMyCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	PlayDamageAnimation();
	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AMyCharacterBase::PlayDamageAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DamageMontage, 1.0f);
}

void AMyCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AMyCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AMyCharacterBase::SetupCharacterWidget(UMyUserWidget* InUserwidget)
{
	UMyHpBarWidget* HpBarWidget = Cast<UMyHpBarWidget>(InUserwidget);
	if (HpBarWidget)
	{
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp(), Stat->GetMaxHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UMyHpBarWidget::UpdateHpBar);
	}
}

void AMyCharacterBase::TakeItem(UMyItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
		SetCurrentWeaponType(InItemData);
	}
}

void AMyCharacterBase::EquipMelee(UMyItemData* InItemData)
{
	UMyMeleeItemData* MeleeItemData = Cast<UMyMeleeItemData>(InItemData);
	if (MeleeItemData)
	{
		if (MeleeItemData->WeaponMesh.IsPending())
		{
			MeleeItemData->WeaponMesh.LoadSynchronous();
		}
		WeaponSocket->SetSkeletalMesh(MeleeItemData->WeaponMesh.Get());
	}

	if (HasAuthority())
	{
		if (MeleeItemData)
		{
			Stat->SetModifierStat(MeleeItemData->ModifierStat);
		}
	}

	SavedItemData = InItemData;
	bIsHoldingWeapon = true;
}

void AMyCharacterBase::EquipGun(UMyItemData* InItemData)
{
	UMyGunItemData* GunItemData = Cast<UMyGunItemData>(InItemData);
	if (GunItemData)
	{
		if (GunItemData->WeaponMesh.IsPending())
		{
			GunItemData->WeaponMesh.LoadSynchronous();
		}
		WeaponSocket->SetSkeletalMesh(GunItemData->WeaponMesh.Get());
	}

	if (HasAuthority())
	{
		if (GunItemData)
		{
			Stat->SetModifierStat(GunItemData->ModifierStat);
		}
	}

	SavedItemData = InItemData;
	bIsHoldingWeapon = true;
}

void AMyCharacterBase::TakeHealItem(float InHealAmount)
{
	Stat->HealHp(InHealAmount);
}

void AMyCharacterBase::PlayShootAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(ShootMontage, 1.0f);
}

int32 AMyCharacterBase::GetLevel()
{
	return Stat->GetCurrentLevel();
}

void AMyCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AMyCharacterBase::ApplyStat(const FMyCharacterStat& BaseStat, const FMyCharacterStat& ModifierStat)
{
	float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AMyCharacterBase::DetachWeapon(UMyItemData* InItemData)
{
	UMyNoWeaponData* ItemData = Cast<UMyNoWeaponData>(InItemData);
	
	if (ItemData)
	{
		if (ItemData->WeaponMesh.IsPending())
		{
			ItemData->WeaponMesh.LoadSynchronous();
		}
		WeaponSocket->SetSkeletalMesh(ItemData->WeaponMesh.Get());
	}
	
	//WeaponSocket->SetVisibility(false);

	if (HasAuthority())
	{
		if (ItemData)
		{
			Stat->SetModifierStat(ItemData->ModifierStat);
		}
	}

	bIsHoldingWeapon = false;
}

EWeaponType AMyCharacterBase::GetCurrentWeaponType()
{
	return CurrentWeaponType;
}

void AMyCharacterBase::SetCurrentWeaponType(UMyItemData* InMyItemData)
{
	if (InMyItemData)
	{
		CurrentWeaponType = InMyItemData->Type;
	}
}

