// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

struct FGenericTeamId;
struct FAttackInfo;
// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKELETONSLAYER_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool  Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData) = 0;
	virtual void  OnDeath() = 0;
	virtual float Heal(float Amount) = 0;
	virtual float GetMaxHP() = 0;
	virtual float GetCurrentHP() = 0;
	virtual float GetMaxStamina() = 0;
	virtual float GetCurrentStamina() = 0;
	virtual bool  IsFriendly(const FGenericTeamId &AttackerTeamId) const = 0;
};
