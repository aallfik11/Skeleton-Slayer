// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class URagdollComponent;
class UCombatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDied);

UCLASS()
class SKELETONSLAYER_API AEnemy : public ACharacter, public IDamageable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void           Tick(float DeltaTime) override;
	virtual void           SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual bool           Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData) override;
	virtual void           OnDeath() override;
	virtual float          Heal(float Amount) override;
	virtual float          GetMaxHP() override;
	virtual float          GetCurrentHP() override;
	virtual float          GetMaxStamina() override;
	virtual float          GetCurrentStamina() override;
	virtual bool           IsFriendly(const FGenericTeamId &AttackerTeamId) const override { return AttackerTeamId == TeamId; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	UPROPERTY(BlueprintAssignable)
	FEnemyDied EnemyDied;

protected:
	// Called when the game starts or when spawned
	virtual void   BeginPlay() override;
	FGenericTeamId TeamId;

private:
	void UpdateEyeColor();
	void DestroyEnemy();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	UCombatComponent *CombatComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	URagdollComponent *RagdollComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Eyes, meta = (AllowPrivateAccess))
	UStaticMeshComponent *RightEye;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Eyes, meta = (AllowPrivateAccess))
	UStaticMeshComponent *LeftEye;
	UPROPERTY()
	UMaterialInstanceDynamic *EyeMaterialInstance;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PHYSICSCORE_API, meta = (AllowPrivateAccess))
	UPhysicsAsset *DismembermentPhysicsAsset;
};
