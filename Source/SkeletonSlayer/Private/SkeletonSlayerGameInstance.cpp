// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonSlayerGameInstance.h"

#include "CombatComponent.h"
#include "Knight.h"
#include "Kismet/GameplayStatics.h"

void USkeletonSlayerGameInstance::SavePlayerState(AKnight *Player)
{
	PlayerMaxHP          = -1;
	PlayerCurrentHP      = -1;
	PlayerMaxStamina     = -1;
	PlayerCurrentStamina = -1;
	PlayerWeapon         = nullptr;

	if (Player)
	{
		PlayerMaxHP          = Player->GetMaxHP() + 25;
		PlayerCurrentHP      = Player->GetCurrentHP() + 25;
		PlayerMaxStamina     = Player->GetMaxStamina();
		PlayerCurrentStamina = Player->GetCurrentStamina();
		if (const UCombatComponent *PlayerCombatComponent = Cast<UCombatComponent>(
			Player->GetComponentByClass(UCombatComponent::StaticClass())))
		{
			if (PlayerCombatComponent->GetWeapon())
			{
				PlayerWeapon = PlayerCombatComponent->GetWeapon()->GetClass();
			}
		}
	}
}

void USkeletonSlayerGameInstance::LoadPlayer()
{
	if (AActor *PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AKnight::StaticClass()))
	{
		if (AKnight *Knight = Cast<AKnight>(PlayerActor))
		{
			if (UActorComponent *Component = Knight->GetComponentByClass(UCombatComponent::StaticClass()))
			{
				if (UCombatComponent *CombatComponent = Cast<UCombatComponent>(Component))
				{
					if (PlayerWeapon)
					{
						AWeapon *Weapon = Cast<AWeapon>(GetWorld()->SpawnActor(PlayerWeapon));
						Weapon->OnInteract_Implementation(PlayerActor);
						CombatComponent->SetMaxHP(PlayerMaxHP);
						CombatComponent->SetCurrentHP(PlayerCurrentHP);
						CombatComponent->SetMaxStamina(PlayerMaxStamina);
						CombatComponent->SetCurrentStamina(PlayerCurrentStamina);
					}
				}
			}
		}
	}
}

void USkeletonSlayerGameInstance::ResetPlayer()
{
	PlayerMaxHP          = -1;
	PlayerCurrentHP      = -1;
	PlayerMaxStamina     = -1;
	PlayerCurrentStamina = -1;
	PlayerWeapon         = nullptr;
}
