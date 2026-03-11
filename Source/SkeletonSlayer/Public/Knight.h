// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Map.h"

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GenericTeamAgentInterface.h"
#include "InputActionValue.h"
#include "SkeletonSlayerTeamId.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Knight.generated.h"

class UCombatComponent;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UCameraComponent;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKnightPickedUpWeapon, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnightDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnightInjured);

UCLASS()
class SKELETONSLAYER_API AKnight : public ACharacter, public IDamageable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKnight();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual bool Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData) override;
	virtual void OnDeath() override;
	virtual float Heal(float Amount) override;
	virtual float GetMaxHP() override;
	virtual float GetCurrentHP() override;
	virtual float GetMaxStamina() override;
	virtual float GetCurrentStamina() override;
	virtual bool IsFriendly(const FGenericTeamId &AttackerTeamId) const override { return AttackerTeamId == TeamId; }
	void EquipWeapon(AWeapon *Weapon);
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

	UPROPERTY(BlueprintAssignable)
	FKnightPickedUpWeapon KnightPickedUpWeapon;
	UPROPERTY(BlueprintAssignable)
	FKnightDied KnightDied;
	UPROPERTY(BlueprintAssignable)
	FKnightInjured KnightInjured;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	 * Input Mappings
	 */

	virtual void Move(const FInputActionValue &Value);
	virtual void Look(const FInputActionValue &Value);
	virtual void Block(const FInputActionValue &Value);
	virtual void StopBlocking(const FInputActionValue &Value);
	virtual void Slash(const FInputActionValue &Value);
	virtual void Overhead(const FInputActionValue &Value);
	virtual void Interact(const FInputActionValue &Value);
	virtual bool CanBeSeenFrom(const FVector &ObserverLocation,
	                           FVector &OutSeenLocation,
	                           int32 &NumberOfLoSChecksPerformed,
	                           float &OutSightStrength,
	                           const AActor *IgnoreActor,
	                           const bool *bWasVisible,
	                           int32 *UserData) const; //IAISightTargetInterface, breaks perception for some reason

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputMappingContext *KnightContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *SlashAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *OverheadAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *BlockAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction *InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequenceBase *IdleAnimation;
	FGenericTeamId     TeamId;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	UCombatComponent *CombatComponent;
	UPROPERTY(EditAnywhere)
	UCameraComponent *Camera;
};
