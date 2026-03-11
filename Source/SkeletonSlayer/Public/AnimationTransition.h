// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "AnimationTransition.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable, BlueprintType)
struct FAnimationTransition : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	;

	FAnimationTransition() : FromState(EAction::None),
	                         ToState(EAction::None),
	                         MontageSectionName("None")
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAction FromState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAction ToState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MontageSectionName;
};
