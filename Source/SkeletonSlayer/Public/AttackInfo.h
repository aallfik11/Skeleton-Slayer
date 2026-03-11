#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "DamageResponse.h"
#include "AttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FAttackInfo() : AttackType(EAction::None),
	                DamageAmount(0.0f),
	                StaminaCost(0.0f),
	                BlockStaminaDamage(0.0f),
	                bIsBlockable(true),
	                bIsParryable(true),
	                bShouldForceInterrupt(false),
	                DamageResponse(EDamageResponse::HitReaction)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	EAction AttackType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	float StaminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	float BlockStaminaDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	bool bIsBlockable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	bool bIsParryable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	bool bShouldForceInterrupt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
	EDamageResponse DamageResponse;
};
