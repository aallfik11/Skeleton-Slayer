// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStateWeaponCollision.h"

#include "CombatComponent.h"

void UAnimNotifyStateWeaponCollision::NotifyBegin(USkeletalMeshComponent *         MeshComp,
                                                  UAnimSequenceBase *              Animation,
                                                  float                            TotalDuration,
                                                  const FAnimNotifyEventReference &EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	OwnerCombatComponent = MeshComp->GetOwner()->GetComponentByClass<UCombatComponent>();
	if (OwnerCombatComponent)
	{
		OwnerCombatComponent->SetIsAttacking(true);
	}
}

void UAnimNotifyStateWeaponCollision::NotifyEnd(USkeletalMeshComponent *         MeshComp,
                                                UAnimSequenceBase *              Animation,
                                                const FAnimNotifyEventReference &EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (OwnerCombatComponent)
	{
		OwnerCombatComponent->SetIsAttacking(false);
		OwnerCombatComponent->ResetTraceState();
	}
}

void UAnimNotifyStateWeaponCollision::NotifyTick(USkeletalMeshComponent *         MeshComp,
                                                 UAnimSequenceBase *              Animation,
                                                 float                            FrameDeltaTime,
                                                 const FAnimNotifyEventReference &EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (OwnerCombatComponent)
	{
		OwnerCombatComponent->RunWeaponTrace();
	}
}
