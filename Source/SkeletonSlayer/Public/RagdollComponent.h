// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackInfo.h"
#include "Components/ActorComponent.h"
#include "RagdollComponent.generated.h"


struct FAttackInfo;
class UPhysicalAnimationComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKELETONSLAYER_API URagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URagdollComponent();
	// Called every frame
	virtual void TickComponent(float                        DeltaTime,
	                           ELevelTick                   TickType,
	                           FActorComponentTickFunction *ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	void ToggleHitReaction(const FHitResult &AttackHitResults);

	UFUNCTION(BlueprintCallable)
	void ToggleRagdoll(const FAttackInfo &AttackInfo, const FHitResult &AttackHitResults);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (AllowPrivateAccess))
	float HitReactionTimeRemaining;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	UPhysicalAnimationComponent *PhysicalAnimationComponent;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	USkeletalMeshComponent *Mesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	UPhysicsAsset *GibPhysicsAsset;
	UFUNCTION(BlueprintCallable)
	void BreakIntoPieces(const FAttackInfo &AttackInfo, const FHitResult &AttackHitResults);
};
