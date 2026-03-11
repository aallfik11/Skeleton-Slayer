// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Actor.h"
#include "TargetDummy.generated.h"

UCLASS()
class SKELETONSLAYER_API ATargetDummy : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetDummy();
	virtual bool  Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData) override;
	virtual void  OnDeath() override;
	virtual float Heal(float Amount) override;
	virtual float GetMaxHP() override;
	virtual float GetCurrentHP() override;
	virtual float GetMaxStamina() override;
	virtual float GetCurrentStamina() override;
	virtual bool  IsFriendly(const FGenericTeamId &AttackerTeamId) const override { return false; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent *Mesh;

};
