// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonManager.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASkeletonManager::ASkeletonManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	EnemiesRemaining = -1;
}

// Called when the game starts or when spawned
void ASkeletonManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	EnemiesRemaining = Enemies.Num();
	for(AActor* Actor: Enemies)
	{
		if(AEnemy* Enemy = Cast<AEnemy>(Actor))
		{
			Enemy->EnemyDied.AddDynamic(this, &ASkeletonManager::HandleEnemyDeath);
		}
	}
}

void ASkeletonManager::HandleEnemyDeath()
{
	--EnemiesRemaining;
	if(EnemiesRemaining == 0)
	{
		AllEnemiesDied.Broadcast();
	}
}

// Called every frame
void ASkeletonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

