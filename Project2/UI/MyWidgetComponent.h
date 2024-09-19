// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MyWidgetComponent.generated.h"

/**
 * OwningPawn�� ���� ������ Owning Actor ����
 * 
 * ���� �󿡼� 2D HUD�� �ƴ� 3D ���忡 widget�� �����ֱ� ���ؼ��� ������Ʈ �ʿ�
 */
UCLASS()
class PROJECT2_API UMyWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;
	
};
