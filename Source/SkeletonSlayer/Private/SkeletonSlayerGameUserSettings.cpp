// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonSlayerGameUserSettings.h"

USkeletonSlayerGameUserSettings::USkeletonSlayerGameUserSettings(const FObjectInitializer &ObjectInitializer) :
	Super(ObjectInitializer)
{
	GammaValue = 1.0f;
}

void USkeletonSlayerGameUserSettings::SetGammaValue(float NewGamma)
{
	GammaValue = FMath::Clamp(NewGamma, 0.0f, 2.0f);
}

USkeletonSlayerGameUserSettings *USkeletonSlayerGameUserSettings::GetSkeletonSlayerGameUserSettings()
{
	return Cast<USkeletonSlayerGameUserSettings>(GetGameUserSettings());
}
