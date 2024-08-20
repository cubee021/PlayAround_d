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
	/** �÷��̾� ȭ�鿡 �������� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyHUDWidget> MyHUDWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyHUDWidget;
	/** ���� ������ �������� ��� ȭ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
		TSubclassOf<class UMyMatchEndWidget> MyMatchEndWidgetClass;

public:
	/** Widget that will show when match end */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
		TObjectPtr<class UUserWidget> MyMatchEndWidget;

protected:
	void ShowMatchEndWidget();

};
