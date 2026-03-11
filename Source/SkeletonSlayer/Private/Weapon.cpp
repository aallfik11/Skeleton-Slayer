// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/BoxComponent.h"
#include "Knight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh                    = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(WeaponMesh);
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Swing Trace Start Point"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Swing Trace End Point"));
	TraceEnd->SetupAttachment(GetRootComponent());
	ClashSound        = nullptr;
	PickupSound       = nullptr;
	WeaponAnimMontage = nullptr;
	IdleAnimation     = nullptr;
	AttachSocketName  = TEXT("hand_r_socket");
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnInteract_Implementation(AActor *Interactor)
{
	if (AKnight *Knight = Cast<AKnight>(Interactor))
	{
		Knight->EquipWeapon(this);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation(), FRotator(0));
		}
	}
	OnWeaponPickedUp.Broadcast(this);
}
