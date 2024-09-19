// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MyWidgetComponent.generated.h"

/**
 * OwningPawn이 없는 위젯에 Owning Actor 설정
 * 
 * 게임 상에서 2D HUD가 아닌 3D 월드에 widget을 보여주기 위해서는 컴포넌트 필요
 */
UCLASS()
class PROJECT2_API UMyWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;
	
};
