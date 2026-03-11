// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonAIController.h"

#include "Enemy.h"
#include "SkeletonSlayerTeamId.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"


// Sets default values
ASkeletonAIController::ASkeletonAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TeamId                        = ESkeletonSlayerTeamId::None;
}

// Called when the game starts or when spawned
void ASkeletonAIController::BeginPlay()
{
	Super::BeginPlay();
	if (AEnemy *OwningActor = Cast<AEnemy>(GetOwner()))
	{
		Agent  = OwningActor;
		TeamId = OwningActor->GetGenericTeamId();
	}
}

ETeamAttitude::Type ASkeletonAIController::GetTeamAttitudeTowards(const AActor &Other) const
{
	const APawn *OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	const IGenericTeamAgentInterface *PlayerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(&Other);
	const IGenericTeamAgentInterface *AITeamAgentInterface     = Cast<IGenericTeamAgentInterface>(
		OtherPawn->GetController());

	if (PlayerTeamAgentInterface == nullptr && AITeamAgentInterface == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	FGenericTeamId OtherTeamID = ESkeletonSlayerTeamId::None;

	if (AITeamAgentInterface)
	{
		OtherTeamID = AITeamAgentInterface->GetGenericTeamId();
	}
	else if (PlayerTeamAgentInterface)
	{
		OtherTeamID = PlayerTeamAgentInterface->GetGenericTeamId();
	}

	if (OtherTeamID == TeamId)
	{
		return ETeamAttitude::Friendly;
	}
	else if (OtherTeamID == ESkeletonSlayerTeamId::Knight)
	{
		return ETeamAttitude::Hostile;
	}
	else
	{
		return ETeamAttitude::Neutral;
	}
}

// Called every frame
void ASkeletonAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
