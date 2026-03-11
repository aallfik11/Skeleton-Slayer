// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SkeletonAIController.generated.h"


class AEnemy;

UCLASS()
class SKELETONSLAYER_API ASkeletonAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASkeletonAIController();
	virtual void           Tick(float DeltaTime) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:
	virtual void                BeginPlay() override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor &Other) const override;

	FGenericTeamId TeamId;
	UPROPERTY(BlueprintReadWrite)
	AEnemy *Agent;


};
