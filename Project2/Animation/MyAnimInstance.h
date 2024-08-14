// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Project2/Item/MyItemData.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMyAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		TObjectPtr<class AMyCharacterPlayer> Owner;

	/** Get Owner's CharacterMovement to update player's current movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		TObjectPtr<class UCharacterMovementComponent> MovementComponent;

protected:
	/** 
	 * Variables that will be read in ABP_MyCharacter
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		FVector Velocity;

	// �̵� �ӵ� �ִ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		float MovingThreshold;

	// ���� ���� �ִ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		float JumpingThreshold;

	// ���� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		uint8 bIsIdle : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		uint8 bIsJumping : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
		EWeaponType WeaponType;
	
};
