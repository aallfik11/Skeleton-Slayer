// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "AttackInfo.h"
#include "CombatComponent.h"
#include "RagdollComponent.h"
#include "SkeletonSlayerTeamId.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CombatComponent               = CreateDefaultSubobject<UCombatComponent>("Combat Component");
	RagdollComponent              = CreateDefaultSubobject<URagdollComponent>("Ragdoll Component");

	RightEye = CreateDefaultSubobject<UStaticMeshComponent>("Right Eye");
	LeftEye  = CreateDefaultSubobject<UStaticMeshComponent>("Left Eye");

	RightEye->SetupAttachment(GetMesh());
	LeftEye->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(
		TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	RightEye->SetStaticMesh(SphereMeshAsset.Object);
	LeftEye->SetStaticMesh(SphereMeshAsset.Object);

	RightEye->ComponentTags.AddUnique(FName("eye_r"));
	LeftEye->ComponentTags.AddUnique(FName("eye_l"));

	//Setup modular bones
	if (GetMesh())
	{
		TArray<USceneComponent *> Bones;
		GetMesh()->GetChildrenComponents(true, Bones);
		for (const auto &Bone : Bones)
		{
			Bone->AttachToComponent(GetMesh(),
			                        FAttachmentTransformRules(EAttachmentRule::KeepWorld,
			                                                  EAttachmentRule::KeepWorld,
			                                                  EAttachmentRule::KeepRelative,
			                                                  true),
			                        Bone->ComponentTags[0]);
		}
	}

	DismembermentPhysicsAsset = nullptr;
	TeamId = ESkeletonSlayerTeamId::Skeleton;
}

bool AEnemy::Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData)
{
	const bool bDidTakeDamage = CombatComponent->TakeDamage(AttackInfo, AttackTraceHitData);
	if (bDidTakeDamage)
	{
		UpdateEyeColor();
		if (CombatComponent->IsDead())
		{
			RagdollComponent->ToggleRagdoll(AttackInfo, AttackTraceHitData);
			//TODO move to ondeath
			//TODO end
			OnDeath();
		}
		else
		{
			RagdollComponent->ToggleHitReaction(AttackTraceHitData);
		}

	}
	return bDidTakeDamage;
}

void AEnemy::OnDeath()
{
	RightEye->DestroyComponent();
	LeftEye->DestroyComponent();
	FTimerHandle DeathHandle;
	GetWorld()->GetTimerManager().SetTimer(DeathHandle, this, &AEnemy::DestroyEnemy, 5.0f, false);
	EnemyDied.Broadcast();
}

float AEnemy::Heal(float Amount)
{
	return CombatComponent->Heal(Amount);
}

float AEnemy::GetMaxHP()
{
	return CombatComponent->GetMaxHP();
}

float AEnemy::GetCurrentHP()
{
	return CombatComponent->GetCurrentHP();
}

float AEnemy::GetMaxStamina()
{
	return CombatComponent->GetMaxStamina();
}

float AEnemy::GetCurrentStamina()
{
	return CombatComponent->GetCurrentStamina();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EyeMaterialInstance = RightEye->CreateAndSetMaterialInstanceDynamic(0);
	LeftEye->SetMaterial(0, EyeMaterialInstance);
	UpdateEyeColor();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::UpdateEyeColor()
{
	const float            HealthPercentage = GetCurrentHP() / GetMaxHP();
	constexpr FLinearColor Red(1.0f, 0.0f, 0.0f);
	constexpr FLinearColor Green(0.0f, 1.0f, 0.0f);
	FLinearColor           FinalColor;
	if (HealthPercentage <= 0.5f)
	{
		//interpolating between red and yellow
		FinalColor   = Red;
		FinalColor.G = HealthPercentage * 2.0f;
	}
	else
	{
		//interpolating between green and yellow
		FinalColor   = Green;
		FinalColor.R = (1.0f - HealthPercentage) * 2.0f;
	}

	EyeMaterialInstance->SetVectorParameterValue(TEXT("Color"), FinalColor);
}

void AEnemy::DestroyEnemy()
{
	this->Destroy();
}
