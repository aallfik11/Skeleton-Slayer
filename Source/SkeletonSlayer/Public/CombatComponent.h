// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Weapon.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

enum class EAction : uint8;
struct FAttackInfo;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKELETONSLAYER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();
	virtual void TickComponent(float                        DeltaTime,
	                           ELevelTick                   TickType,
	                           FActorComponentTickFunction *ThisTickFunction) override;

	/**
	 * 
	 * @param AttackInfo - Information about the attack
	 * @param AttackTraceHitData - Information about the trace that caused damage
	 * @return true if damaged, false otherwise
	 */
	bool TakeDamage(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData);
	void SubtractStaminaCost(const float StaminaCost);
	UFUNCTION(BlueprintCallable)
	float Heal(float Amount);
	UFUNCTION(BlueprintCallable)
	void ResetTraceState();
	UFUNCTION(BlueprintCallable)
	void ProcessInput(EAction Action);
	UFUNCTION(BlueprintCallable)
	void PerformQueuedAction();

	/**
	 * Ran each tick off of AnimNotifyStateWeaponCollision. Performs weapon tracing and deals damage when applicable
	 */
	UFUNCTION(BlueprintCallable)
	void RunWeaponTrace();
	
	/**
	 * @param NewWeapon A new weapon to be equipped
	 */
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeapon *NewWeapon);

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AWeapon> DefaultWeapon;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void    LoadAnimDataTable();
	void    RegenerateStamina();
	FVector CalculateIntermediateTracePoint(const int32    TracePointIndex,
	                                        const FVector &TraceStart,
	                                        const FVector &TraceEnd) const;
	void CalculateTracePrecision();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health and Stamina", meta = (AllowPrivateAccess))
	float MaxHP;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Health and Stamina", meta = (AllowPrivateAccess))
	float CurrentHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health and Stamina", meta = (AllowPrivateAccess))
	float MaxStamina;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Health and Stamina", meta = (AllowPrivateAccess))
	float CurrentStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health and Stamina", meta = (AllowPrivateAccess))
	float StaminaRegenerationRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bCanRegenerateStamina;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bIsAttacking;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bIsParrying;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bIsBlocking;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bCanAct;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bIsDead;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Character State", meta = (AllowPrivateAccess))
	bool bIsAltAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	AWeapon *Weapon;
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UDataTable *WeaponAttackDataTable;
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UDataTable *WeaponAnimationTransitionDataTable;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character State", meta = (AllowPrivateAccess))
	EAction CurrentAction;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character State", meta = (AllowPrivateAccess))
	EAction NextAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta = (AllowPrivateAccess))
	int32 TracePrecision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta = (AllowPrivateAccess))
	bool bAutoDetermineTracePrecision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta = (AllowPrivateAccess))
	bool bDrawDebugTraces;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta = (AllowPrivateAccess))
	float TraceBoxHalfSize;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sounds, meta = (AllowPrivateAccess))
	USoundBase *HitSound;

	/**
	 * Contains montage section names mapped to appropriate EAction combinations
	 */
	TMap<EAction, TMap<EAction, FName>> ActionTransitions;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character State", meta = (AllowPrivateAccess))
	FTimerHandle StaminaRegenTimerHandle;

	UPROPERTY()
	TArray<AActor *>     IgnoreActors;
	TMap<uint8, FVector> PreviousTracePoints;

	FGenericTeamId OwnerTeamId;

public:
	float GetMaxHP() const
	{
		return MaxHP;
	}

	void SetMaxHP(const float NewMaxHP)
	{
		this->MaxHP = NewMaxHP;
	}

	float GetCurrentHP() const
	{
		return CurrentHP;
	}

	void SetCurrentHP(const float NewCurrentHP)
	{
		this->CurrentHP = NewCurrentHP;
	}

	float GetMaxStamina() const
	{
		return MaxStamina;
	}

	void SetMaxStamina(float NewMaxStamina)
	{
		MaxStamina = NewMaxStamina;
	}

	float GetCurrentStamina() const
	{
		return CurrentStamina;
	}

	void SetCurrentStamina(float NewCurrentStamina)
	{
		CurrentStamina = NewCurrentStamina;
	}

	bool CanRegenerateStamina() const
	{
		return bCanRegenerateStamina;
	}

	void SetCanRegenerateStamina(bool bNewCanRegenerateStamina)
	{
		bCanRegenerateStamina = bNewCanRegenerateStamina;
	}

	bool IsAttacking() const
	{
		return bIsAttacking;
	}

	void SetIsAttacking(const bool bNewIsAttacking)
	{
		this->bIsAttacking = bNewIsAttacking;
	}

	bool IsParrying() const
	{
		return bIsParrying;
	}

	void SetIsParrying(const bool bNewIsParrying)
	{
		this->bIsParrying = bNewIsParrying;
	}

	bool IsBlocking() const
	{
		return bIsBlocking;
	}

	void SetIsBlocking(const bool bNewIsBlocking)
	{
		this->bIsBlocking = bNewIsBlocking;
	}

	bool CanAct() const
	{
		return bCanAct;
	}

	void SetCanAct(const bool bNewCanAct)
	{
		bCanAct = bNewCanAct;
	}

	bool IsDead() const
	{
		return bIsDead;
	}

	void SetIsDead(const bool bNewIsDead)
	{
		this->bIsDead = bNewIsDead;
	}

	bool IsAltAttack() const
	{
		return bIsAltAttack;
	}

	void SetIsAltAttack(const bool bNewIsAltAttack)
	{
		this->bIsAltAttack = bNewIsAltAttack;
	}

	AWeapon *GetWeapon() const
	{
		return Weapon;
	}

	UDataTable *GetWeaponDataTable() const
	{
		return WeaponAttackDataTable;
	}

	void SetWeaponAttackDataTable(UDataTable *const NewWeaponAttackDataTable)
	{
		this->WeaponAttackDataTable = NewWeaponAttackDataTable;
	}

	void SetWeaponAnimationDataTable(UDataTable *const NewWeaponAnimationDataTable)
	{
		this->WeaponAnimationTransitionDataTable = NewWeaponAnimationDataTable;
	}

	EAction GetCurrentAction() const { return CurrentAction; }

	void SetCurrentAction(EAction Action)
	{
		CurrentAction = Action;
	}

	EAction GetNextAction() const { return NextAction; }

	void SetNextAction(EAction Action)
	{
		NextAction = Action;
	}
};
