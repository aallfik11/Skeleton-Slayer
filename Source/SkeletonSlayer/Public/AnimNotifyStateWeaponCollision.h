// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyStateWeaponCollision.generated.h"

class UCombatComponent;
/**
 * 
 */
UCLASS()
class SKELETONSLAYER_API UAnimNotifyStateWeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent*          MeshComp,
	                         UAnimSequenceBase*               Animation,
	                         float                            TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent*          MeshComp,
	                       UAnimSequenceBase*               Animation,
	                       const FAnimNotifyEventReference& EventReference) override;


	virtual void NotifyTick(USkeletalMeshComponent*          MeshComp,
	                        UAnimSequenceBase*               Animation,
	                        float                            FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY()
	UCombatComponent* OwnerCombatComponent;
};
