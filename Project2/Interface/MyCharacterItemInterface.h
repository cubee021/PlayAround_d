// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MyCharacterItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMyCharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT2_API IMyCharacterItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TakeItem(class UMyItemData* InItemData) = 0;
};
