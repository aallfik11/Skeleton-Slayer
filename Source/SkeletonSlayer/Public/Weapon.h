// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "Weapon.generated.h"

class UBoxComponent;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponPickedUp, AActor*, PickedUpWeapon);

UCLASS(BlueprintType)
class SKELETONSLAYER_API AWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	virtual void              OnInteract_Implementation(AActor *Interactor) override;
	FORCEINLINE UAnimMontage *GetWeaponAnimMontage() const
	{
		return WeaponAnimMontage;
	}

	FORCEINLINE UAnimSequenceBase *GetIdleAnimation() const { return IdleAnimation; }

	UStaticMeshComponent *GetMesh() const { return WeaponMesh; }
	UDataTable *          GetAttackDataTable() const { return WeaponDataTable; }
	UDataTable *          GetAnimationDataTable() const { return WeaponAnimationTransitionDataTable; }
	USceneComponent *     GetStartTracePoint() const { return TraceStart; }
	USceneComponent *     GetEndTracePoint() const { return TraceEnd; }
	USoundBase *          GetClashSound() const { return ClashSound; }
	const FName &         GetAttackSocketName() const { return AttachSocketName; }

	UPROPERTY(BlueprintAssignable)
	FOnWeaponPickedUp OnWeaponPickedUp;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	UStaticMeshComponent *WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent *WeaponBox;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent *TraceStart;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent *TraceEnd;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	UAnimMontage *WeaponAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	UAnimSequenceBase *IdleAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	USoundBase *ClashSound;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	USoundBase *PickupSound;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	FName AttachSocketName;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	UDataTable *WeaponDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess))
	UDataTable *WeaponAnimationTransitionDataTable;
};
