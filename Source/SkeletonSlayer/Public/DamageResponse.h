// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(Blueprintable, BlueprintType)
enum class EDamageResponse : uint8
{
    None,
    HitReaction,
    Stagger,
    Stun,
    KnockBack
};
