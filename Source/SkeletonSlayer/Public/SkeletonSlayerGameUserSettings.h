// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SkeletonSlayerGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class SKELETONSLAYER_API USkeletonSlayerGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	USkeletonSlayerGameUserSettings(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Brightness Settings")
	void SetGammaValue(float NewGamma);
	UFUNCTION(BlueprintPure, Category = "Brightness Settings")
	float GetGammaValue() const { return GammaValue; }

	UFUNCTION(BlueprintCallable)
	static USkeletonSlayerGameUserSettings *GetSkeletonSlayerGameUserSettings();

private:
	UPROPERTY(Config)
	float GammaValue;
};
