// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkeletonSlayerGameInstance.generated.h"

/**
 * 
 */

class AKnight;
class AWeapon;

UCLASS()
class SKELETONSLAYER_API USkeletonSlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SavePlayerState(AKnight *Player);
	UFUNCTION(BlueprintCallable)
	void LoadPlayer();
	UFUNCTION(BlueprintCallable)
	void ResetPlayer();
	UFUNCTION(BlueprintPure)
	int32 GetPlayerMaxHP() const { return PlayerMaxHP; }

	UFUNCTION(BlueprintPure)
	int32 GetPlayerCurrentHP() const { return PlayerCurrentHP; }

	UFUNCTION(BlueprintPure)
	TSubclassOf<AWeapon> GetPlayerWeapon() const { return PlayerWeapon; }

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerMaxHP;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerCurrentHP;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerMaxStamina;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerCurrentStamina;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	TSubclassOf<AWeapon> PlayerWeapon;
};
