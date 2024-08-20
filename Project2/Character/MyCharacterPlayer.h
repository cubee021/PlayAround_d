// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacterBase.h"
#include "InputActionValue.h"
#include "MyCharacterPlayer.generated.h"

/**
 * Manages functions for player such as : Camera, input action, networking
 */
UCLASS()
class PROJECT2_API AMyCharacterPlayer : public AMyCharacterBase
{
	GENERATED_BODY()
	
public:
	AMyCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void SetDead() override;

public:
	FTimerHandle DeadTimerHandle;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/**
	* Character control section */
	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	/** Set spring data from control data */
	virtual void SetCharacterControlData(const class UMyCharacterControlData* CharacterControlData) override;

protected:
	/** Camera section */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UCameraComponent> FollowCamera;

protected:
	/**
	* Input section */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> FirstPersonMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> ThirdPersonMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> FirstPersonLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UInputAction> DropItemAction;

	void FirstPersonMove(const FInputActionValue& Value);
	void FirstPersonLook(const FInputActionValue& Value);

	/** Client notify Server to update this client's rotation yaw
	* NetMulticast is already set on Server */
	UFUNCTION(Server, Reliable)
		void ServerRPCReplicateYaw(double InNewRotationYaw);

	void ThirdPersonMove(const FInputActionValue& Value);

	ECharacterControlType CurrentCharacterControlType;

protected:
	/** Bind different actions depend on weapon type */
	void WeaponKeyHandler();

protected:
	/**
	* Attack section */
	void Attack();
	void PlayAttackAnimation();
	virtual void AttackHitCheck() override;
	void AttackHitConfirm(AActor* HitActor);

	///////////////////////////////////////////////////////
	// NoWeapon(punch), Melee networking
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Client notify Server a client attack. Input client's attack start time */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCAttack(float AttackStartTime);
	/** Notify everyone to play animation of this player */
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRPCAttack();

	UFUNCTION(Client, Unreliable)
		void ClientRPCPlayAnimation(AMyCharacterPlayer* CharacterToPlay);
	/** hit 판정은 서버에서 진행 */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCNotifyMiss(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime);

	UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
		uint8 bCanAttack : 1;

	UFUNCTION()
		void OnRep_CanAttack();

	// 공격 동작 진행시간
	float AttackTime = 1.f;
	// 클라이언트가 마지막으로 공격한 시점
	float LastAttackStartTime = 0.f;
	// 클라이언트와 서버의 시간차
	float AttackTimeDifference = 0.f;
	// 두 플레이어 간의 거리
	float AcceptCheckDistance = 300.0f;
	// 최소 판정 시간 : 서버가 너무 빨리 hit 판정 내리는 것을 방지 
	float AcceptMinCheckTime = 0.15f;

public:
	FTimerHandle AttackTimerHandle;
	///////////////////////////////////////////////////////

public:
	void ResetPlayer();
	void ResetAttack();
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	/**
	* Gun fire section */
	UFUNCTION()
		void Fire();

	/** Bullet spawn location on first player screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
		FVector MuzzleOffset;
	/** Bullet projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
		TSubclassOf<class AActor> ProjectileClass;

protected:
	///////////////////////////////////////////////////////
	// Gun fire networking
	void TakeDamageFromBullet(AActor* DamagedCharacter);

	/** 클라이언트가 총을 쐈다고 서버로 알리면 서버에서 해당 클라이언트의 fire() 실행.
	* 모든 플레이어에게 bullet projectile이 보이며, 판정이 정확함.
	* 클라이언트의 fire animation과 서버의 fire()의 타이밍을 맞추려면 ServerRPCFire_Validate()를
	* 만들어야 하는데 어려워서 보류.. */
	UFUNCTION(Server, Reliable)
		void ServerRPCFire();
	UFUNCTION(Client, Unreliable)
		void ClientRPCPlayFireAnimation(AMyCharacterPlayer* CharacterToPlay);
	/** Client notify Server that client got a shot */
	UFUNCTION(Server, Reliable)
		void ServerRPCNotifyBulletHit(AActor* DamagedCharacter);
	///////////////////////////////////////////////////////

protected:
	UFUNCTION()
		void DropItem();

	/** Item spawn location on first player screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
		FVector SpawnOffset;
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Gun)
		TSubclassOf<class AActor> ItemProjectileClass;

	UFUNCTION(Server, Reliable)
		void ServerRPCDropItem();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastRPCDropItem();

};
