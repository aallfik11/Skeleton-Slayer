// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(Blueprintable, BlueprintType)
enum class EAction : uint8 
{
    None,
    Idle,
    Block,
    Parry,
    Interact,
    LeftSlash,
    RightSlash,
    LeftOverhead,
    RightOverhead,
    Special1,
    Special2,
    Special3,
    Special4,
};
