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
	/** 플레이어 화면에 보여지는 위젯 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyHUDWidget> MyHUDWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyHUDWidget;
	/** 게임 끝나면 보여지는 결과 화면 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyMatchEndWidget> MyMatchEndWidgetClass;

public:
	/** Widget that will show when match end */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyMatchEndWidget;

protected:
	void ShowMatchEndWidget();

};
