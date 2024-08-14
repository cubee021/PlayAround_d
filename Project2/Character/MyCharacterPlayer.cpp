// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyCharacterControlData.h"
#include "MyCharacterStatComponent.h"
#include "Project2/Interface/MyGameInterface.h"
#include "Project2/Weapon/MyBullet.h"
#include "Project2/Physics/MyCollision.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Engine/DamageEvents.h"
#include "Item/MyDropItem.h"


AMyCharacterPlayer::AMyCharacterPlayer()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_Jump.IA_Jump'"));
	if (IA_JumpRef.Object)
	{
		JumpAction = IA_JumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ChangeControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (IA_ChangeControlRef.Object)
	{
		ChangeControlAction = IA_ChangeControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_FirstPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_FirstPersonMove.IA_FirstPersonMove'"));
	if(IA_FirstPersonMoveRef.Object)
	{
		FirstPersonMoveAction = IA_FirstPersonMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ThirdPersonMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_ThirdPersonMove.IA_ThirdPersonMove'"));
	if (IA_ThirdPersonMoveRef.Object)
	{
		ThirdPersonMoveAction = IA_ThirdPersonMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_FirstPersonLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_FirstPersonLook.IA_FirstPersonLook'"));
	if (IA_FirstPersonLookRef.Object)
	{
		FirstPersonLookAction = IA_FirstPersonLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_Attack.IA_Attack'"));
	if (IA_AttackRef.Object)
	{
		AttackAction = IA_AttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DropItemRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Project2/Input/Actions/IA_DropItem.IA_DropItem'"));
	if (IA_DropItemRef.Object)
	{
		DropItemAction = IA_DropItemRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::ThirdPerson;
	bCanAttack = true;
}

void AMyCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterControlType);
}

void AMyCharacterPlayer::SetDead()
{
	Super::SetDead();

	GetWorldTimerManager().SetTimer(DeadTimerHandle, this, &AMyCharacterPlayer::ResetPlayer, 5.f, false);
}

void AMyCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(FirstPersonMoveAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::FirstPersonMove);
	EnhancedInputComponent->BindAction(ThirdPersonMoveAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::ThirdPersonMove);
	EnhancedInputComponent->BindAction(FirstPersonLookAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::FirstPersonLook);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::WeaponKeyHandler);
	EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &AMyCharacterPlayer::DropItem);
}

void AMyCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::ThirdPerson)
	{
		SetCharacterControl(ECharacterControlType::FirstPerson);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::FirstPerson)
	{
		SetCharacterControl(ECharacterControlType::ThirdPerson);
	}
}

void AMyCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UMyCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// 현재 적용된 input mapping 삭제 후 다시 적용 
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AMyCharacterPlayer::SetCharacterControlData(const UMyCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	SpringArm->TargetArmLength = CharacterControlData->TargetArmLength;
	SpringArm->SetRelativeLocation(CharacterControlData->RelativeLocation);
	SpringArm->SetRelativeRotation(CharacterControlData->RelativeRotation);
	SpringArm->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	SpringArm->bInheritPitch = CharacterControlData->bInheritPitch;
	SpringArm->bInheritRoll = CharacterControlData->bInheritRoll;
	SpringArm->bInheritYaw = CharacterControlData->bInheritYaw;
	SpringArm->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AMyCharacterPlayer::FirstPersonMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AMyCharacterPlayer::ThirdPersonMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AMyCharacterPlayer::FirstPersonLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X * -1.f);
	AddControllerPitchInput(LookAxisVector.Y);

	if (!HasAuthority())
	{
		ServerRPCReplicateYaw(GetActorRotation().Yaw);
	}
}

void AMyCharacterPlayer::ServerRPCReplicateYaw_Implementation(double InNewRotationYaw)
{
	FRotator NewRotation(GetActorRotation().Pitch, InNewRotationYaw, GetActorRotation().Roll);

	SetActorRotation(NewRotation);
}

void AMyCharacterPlayer::WeaponKeyHandler()
{
	if (GetCurrentWeaponType() == EWeaponType::Melee || GetCurrentWeaponType() == EWeaponType::NoWeapon)
	{
		Attack();
	}
	else if (GetCurrentWeaponType() == EWeaponType::Gun)
	{
		Fire();
	}
}

void AMyCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacterPlayer, bCanAttack);
}

void AMyCharacterPlayer::Attack()
{
	//ProcessComboCommand();

	if (bCanAttack)
	{
		if (!HasAuthority())
		{
			bCanAttack = false;
			if (!GetCharacterMovement()->IsFalling())
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			}

			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacterPlayer::ResetAttack, AttackTime, false);

			PlayAttackAnimation();
		}

		ServerRPCAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}
}

void AMyCharacterPlayer::PlayAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.f);

	if (GetCurrentWeaponType() == EWeaponType::NoWeapon)
	{
		AnimInstance->Montage_Play(ComboPunchMontage);
	}
	else if (GetCurrentWeaponType() == EWeaponType::Melee)
	{
		AnimInstance->Montage_Play(ComboMeleeMontage);
	}
}

void AMyCharacterPlayer::AttackHitCheck()
{
	if (IsLocallyControlled())
	{
		FHitResult OutHitResult;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

		const float AttackRange = Stat->GetTotalStat().AttackRange;
		const float AttackRadius = Stat->GetAttackRadius();
		const float AttackDamage = Stat->GetTotalStat().Attack;
		const FVector Forward = GetActorForwardVector();
		const FVector Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + GetActorForwardVector() * AttackRange;

		bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_MYATTACKHIT, FCollisionShape::MakeSphere(AttackRadius), Params);
		
		float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		if (!HasAuthority())
		{
			if (HitDetected)
			{
				ServerRPCNotifyHit(OutHitResult, HitCheckTime);
			}
			else
			{
				ServerRPCNotifyMiss(Start, End, Forward, HitCheckTime);
			}
		}
		else
		{
			if (HitDetected)
			{
				AttackHitConfirm(OutHitResult.GetActor());
			}
		}
		
	}
}

void AMyCharacterPlayer::AttackHitConfirm(AActor* HitActor)
{
	if (HasAuthority())
	{
		const float AttackDamage = Stat->GetTotalStat().Attack;
		FDamageEvent DamageEvent;
		HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}
}

void AMyCharacterPlayer::MulticastRPCAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		PlayAttackAnimation();
	}
}

void AMyCharacterPlayer::ClientRPCPlayAnimation_Implementation(AMyCharacterPlayer* CharacterToPlay)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->PlayAttackAnimation();
	}
}

bool AMyCharacterPlayer::ServerRPCAttack_Validate(float AttackStartTime)
{
	if (LastAttackStartTime == 0.f)
	{
		return true;
	}

	// 플레이어의 공격 중복을 방지
	return (AttackStartTime - LastAttackStartTime) > (AttackTime - 0.4f);
}

void AMyCharacterPlayer::ServerRPCAttack_Implementation(float AttackStartTime)
{
	bCanAttack = false;
	OnRep_CanAttack();

	AttackTimeDifference = GetWorld()->GetTimeSeconds() - AttackStartTime;
	// AttackTimeDifference == AttackTime이어도 타이머가 실행되도록 -0.01f
	AttackTimeDifference = FMath::Clamp(AttackTimeDifference, 0.f, AttackTime - 0.01f);

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacterPlayer::ResetAttack, AttackTime - AttackTimeDifference, false);

	LastAttackStartTime = AttackStartTime;
	PlayAttackAnimation();

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AMyCharacterPlayer* OtherPlayer = Cast<AMyCharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCPlayAnimation(this);
				}
			}
		}
	}
}

void AMyCharacterPlayer::ResetAttack()
{
	bCanAttack = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

float AMyCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Stat->GetCurrentHp() <= 0.f)
	{
		IMyGameInterface* MyGameMode = GetWorld()->GetAuthGameMode<IMyGameInterface>();
		if (MyGameMode)
		{
			MyGameMode->OnPlayerKilled(EventInstigator, GetController(), this);
		}
	}

	return 0.f;
}

bool AMyCharacterPlayer::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return (HitCheckTime - LastAttackStartTime) > AcceptMinCheckTime;
}

void AMyCharacterPlayer::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	AActor* HitActor = HitResult.GetActor();
	if (::IsValid(HitActor))
	{
		const FVector HitLocation = HitResult.Location;
		const FBox HitBox = HitActor->GetComponentsBoundingBox();
		const FVector ActorBoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;
		if (FVector::DistSquared(HitLocation, ActorBoxCenter) <= AcceptCheckDistance * AcceptCheckDistance)
		{
			AttackHitConfirm(HitActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitTest Rejected!"));
		}
#if ENABLE_DRAW_DEBUG
		DrawDebugPoint(GetWorld(), ActorBoxCenter, 10.f, FColor::Cyan, false, 3.f);
		DrawDebugPoint(GetWorld(), HitLocation, 10.f, FColor::Magenta, false, 3.f);
#endif

	}
}

bool AMyCharacterPlayer::ServerRPCNotifyMiss_Validate(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{
	return (HitCheckTime - LastAttackStartTime) > AcceptMinCheckTime;
}

void AMyCharacterPlayer::ServerRPCNotifyMiss_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{

}

void AMyCharacterPlayer::OnRep_CanAttack()
{
	if (!bCanAttack)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void AMyCharacterPlayer::ResetPlayer()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.f);
		Stat->SetLevelStat(1);
		Stat->ResetStat();
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		SetActorEnableCollision(true);
		HpBar->SetHiddenInGame(false);
		
		if (HasAuthority())
		{
			IMyGameInterface* MyGameMode = GetWorld()->GetAuthGameMode<IMyGameInterface>();
			if (MyGameMode)
			{
				FTransform NewTransform = MyGameMode->GetRandomStartTransform();
				TeleportTo(NewTransform.GetLocation(), NewTransform.GetRotation().Rotator());
			}
		}
		
	}
}

void AMyCharacterPlayer::Fire()
{
	PlayShootAnimation();

	if (!HasAuthority())
	{
		ServerRPCFire();
	}
	else
	{
		if (ProjectileClass)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
			FRotator MuzzleRotation = CameraRotation;

			MuzzleRotation.Pitch += 10.0f;
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				//SpawnParams.Instigator = Instigator;

				AMyBullet* Projectile = World->SpawnActor<AMyBullet>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					FVector LaunchDirection = MuzzleRotation.Vector();
					Projectile->FireInDirection(LaunchDirection, Controller, Stat->GetTotalStat().Attack);
					// 총상 입은 플레이어가 damage를 자신과 서버에 알리기 위해
					Projectile->OnBulletHit.AddUObject(this, &AMyCharacterPlayer::TakeDamageFromBullet);
				}
			}

		}
	}
	
}

void AMyCharacterPlayer::TakeDamageFromBullet(AActor* DamagedCharacter)
{
	if (DamagedCharacter)
	{
		if (!HasAuthority())
		{
			ServerRPCNotifyBulletHit(DamagedCharacter);
		}
		else
		{
			AttackHitConfirm(DamagedCharacter);
		}
	}
}

void AMyCharacterPlayer::DropItem()
{
	if (!bIsHoldingWeapon)
	{
		return;
	}

	if (!HasAuthority())
	{
		//ServerRPCFire();
	}
	else
	{
		if (ItemProjectileClass)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
			FRotator MuzzleRotation = CameraRotation;

			MuzzleRotation.Pitch += 10.0f;
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				AMyDropItem* Projectile = World->SpawnActor<AMyDropItem>(ItemProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					Projectile->SetItemToDrop(SavedItemData);

					FVector LaunchDirection = MuzzleRotation.Vector();
					if (Projectile->FireInDirection(LaunchDirection))
					{
						TakeItem(NoWeaponData);
					}


					// 총상 입은 플레이어가 damage를 자신과 서버에 알리기 위해
					//Projectile->OnBulletHit.AddUObject(this, &AMyCharacterPlayer::TakeDamageFromBullet);
				}
			}

		}
	}
}

void AMyCharacterPlayer::ServerRPCNotifyBulletHit_Implementation(AActor* DamagedCharacter)
{
	if (::IsValid(DamagedCharacter))
	{
		AttackHitConfirm(DamagedCharacter);
	}
}

void AMyCharacterPlayer::ServerRPCFire_Implementation()
{
	PlayShootAnimation();
	
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AMyCharacterPlayer* OtherPlayer = Cast<AMyCharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCPlayFireAnimation(this);
				}
			}
		}
	}

	// 서버가 해당 클라이언트의 fire() 실행
	Fire();
}

void AMyCharacterPlayer::ClientRPCPlayFireAnimation_Implementation(AMyCharacterPlayer* CharacterToPlay)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->PlayShootAnimation();
	}
}


