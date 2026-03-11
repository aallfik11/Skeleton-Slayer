// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(Blueprintable, BlueprintType)
enum class EAttackType : uint8 
{
    None,
    RightSlash,
    RightOverhead,
    LeftSlash,
    LeftOverhead,
    Special1,
    Special2,
    Special3,
    Special4,
};
