// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkeletonManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllEnemiesDied);

UCLASS()
class SKELETONSLAYER_API ASkeletonManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletonManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FAllEnemiesDied AllEnemiesDied;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleEnemyDeath();
	
	UPROPERTY(VisibleAnywhere)
	int32 EnemiesRemaining;
	

};
