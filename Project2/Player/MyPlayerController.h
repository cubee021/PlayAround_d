// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyPlayerController();

	virtual void BeginPlay() override;

protected:
	//위젯 더 추가하려면 하나의 HUDWidget에 모아 사용하는게 바람직

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyMatchTimerWidget> MyTimerWidgetClass;
	/** Timer widget on top of the screen */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyTimerWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyMatchEndWidget> MyMatchEndWidgetClass;

public:
	/** Widget that will show when match end */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyMatchEndWidget;

protected:
	void ShowMatchEndWidget();

};
