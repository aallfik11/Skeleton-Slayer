// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetDummy.h"


// Sets default values
ATargetDummy::ATargetDummy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh                          = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
}

bool ATargetDummy::Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, TEXT("Ouch!"));
	}
	return true;
}

void ATargetDummy::OnDeath()
{
}

float ATargetDummy::Heal(float Amount)
{
	return 0.0f;
}

float ATargetDummy::GetMaxHP()
{
	return 0.0f;
}

float ATargetDummy::GetCurrentHP()
{
	return 0.0f;
}

float ATargetDummy::GetMaxStamina()
{
	return 0.0f;
}

float ATargetDummy::GetCurrentStamina()
{
	return 0.0f;
}

// Called when the game starts or when spawned
void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
