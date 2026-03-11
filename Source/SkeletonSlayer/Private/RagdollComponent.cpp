// Fill out your copyright notice in the Description page of Project Settings.


#include "RagdollComponent.h"

#include "AttackInfo.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values for this component's properties
URagdollComponent::URagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PhysicalAnimationComponent        = nullptr;
	Mesh                              = nullptr;
	HitReactionTimeRemaining          = 0.0f;
	GibPhysicsAsset                   = nullptr;
	// ...
}


// Called when the game starts
void URagdollComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		PhysicalAnimationComponent = Cast<UPhysicalAnimationComponent>(
			Owner->AddComponentByClass(UPhysicalAnimationComponent::StaticClass(),
			                           false,
			                           Owner->GetTransform(),
			                           false));
		PhysicalAnimationComponent->SetSkeletalMeshComponent(Owner->GetMesh());
		Mesh = Owner->GetMesh();
	}
	// ...

}

void URagdollComponent::BreakIntoPieces(const FAttackInfo &AttackInfo, const FHitResult &AttackHitResults)
{
}


// Called every frame
void URagdollComponent::TickComponent(float                        DeltaTime,
                                      ELevelTick                   TickType,
                                      FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HitReactionTimeRemaining = FMath::FInterpTo(HitReactionTimeRemaining, 0.0f, DeltaTime, 1.5) - DeltaTime * 0.1f;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(9, 0.2f, FColor::Emerald, TEXT("Simulating Physics"));
	}
	if (HitReactionTimeRemaining <= 0.0f)
	{
		HitReactionTimeRemaining = 0.0f;
		Mesh->SetAllBodiesBelowSimulatePhysics(FName("spine_01"), false, true);
		SetComponentTickEnabled(false);
	}
	else
	{
		Mesh->SetAllBodiesBelowPhysicsBlendWeight(FName("spine_01"), FMath::Min(HitReactionTimeRemaining, 1.0f));
	}
	// ...
}

void URagdollComponent::ToggleHitReaction(const FHitResult &AttackHitResults)
{
	SetComponentTickEnabled(true);
	HitReactionTimeRemaining += 1.0;
	PhysicalAnimationComponent->ApplyPhysicalAnimationProfileBelow(FName("spine_01"), FName("Strong"), true);
	Mesh->SetAllBodiesBelowSimulatePhysics(FName("spine_01"), true, true);
	FVector HitDirection = AttackHitResults.TraceEnd - AttackHitResults.Location;
	HitDirection.Normalize();
	//Mesh->AddImpulse(HitDirection * 10000.0f, AttackHitResults.BoneName, true);
	Mesh->AddRadialImpulse(AttackHitResults.Location, 200.0f, 1000.0f, RIF_Constant, true);
	DrawDebugDirectionalArrow(GetOwner()->GetWorld(),
	                          AttackHitResults.Location,
	                          (AttackHitResults.Location + HitDirection * 50.f),
	                          10.0f,
	                          FColor::Silver,
	                          false,
	                          10.0f);

}

void URagdollComponent::ToggleRagdoll(const FAttackInfo &AttackInfo, const FHitResult &AttackHitResults)
{
	FTimerHandle   UnusedHandle;
	FTimerDelegate BoneBreakDelegate;
	BoneBreakDelegate.BindUFunction(this, FName("BreakIntoPieces"), AttackInfo, AttackHitResults);
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, BoneBreakDelegate, 0.5f, false);
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		SetComponentTickEnabled(false);
		PhysicalAnimationComponent->ApplyPhysicalAnimationProfileBelow(FName("pelvis"), FName("Ragdoll"), true);

		Owner->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		TArray<USceneComponent *>      Components;
		TArray<UStaticMeshComponent *> Bones;
		Owner->GetCapsuleComponent()->GetChildrenComponents(true, Components);
		for (auto &Component : Components)
		{
			if (UStaticMeshComponent *Bone = Cast<UStaticMeshComponent>(Component))
			{
				Bones.AddUnique(Bone);
			}
		}

		FVector HitDirection = (AttackHitResults.TraceEnd - AttackHitResults.Location);
		HitDirection.Normalize();
		DrawDebugDirectionalArrow(GetWorld(),
		                          AttackHitResults.Location,
		                          AttackHitResults.Location + (HitDirection * 50.0f),
		                          5.0f,
		                          FColor::Magenta,
		                          false,
		                          10.f);

		for (auto &Bone : Bones)
		{
			Bone->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			Bone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
			Bone->SetSimulatePhysics(true);
			Bone->AddImpulse(HitDirection * AttackInfo.DamageAmount, Bone->GetAttachSocketName());
		}
		Mesh->bPauseAnims = true;
		if (GibPhysicsAsset)
		{
			Mesh->SetPhysicsAsset(GibPhysicsAsset);
		}
		Mesh->SetPhysicsBlendWeight(1.0f);
		Mesh->SetAllBodiesSimulatePhysics(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->AddImpulse(HitDirection * AttackInfo.DamageAmount, Mesh->GetAttachSocketName());
	}
}
