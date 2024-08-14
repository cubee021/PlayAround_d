// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackHitCheck.h"
#include "Project2/Interface/MyAttackHitInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		IMyAttackHitInterface* AttackPawn = Cast<IMyAttackHitInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}