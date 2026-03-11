// Fill out your copyright notice in the Description page of Project Settings.


#include "Knight.h"

#include "Action.h"
#include "CombatComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AKnight::AKnight()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = false;
	AutoPossessPlayer             = EAutoReceiveInput::Player0;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate                  = FRotator(-1.0, -1.0, 360.0);
	GetCharacterMovement()->JumpZVelocity                 = 400.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), "cameraSocket");
	Camera->bUsePawnControlRotation = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));

	HitCameraShake = nullptr;
	IdleAnimation  = nullptr;

	TeamId = ESkeletonSlayerTeamId::Knight;
}

// Called when the game starts or when spawned
void AKnight::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->HideBoneByName("neck", PBO_None);

	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(KnightContext, 0);
		}
	}
}

void AKnight::Move(const FInputActionValue &Value)
{
	const FVector2d MovementVector  = Value.Get<FVector2d>();
	const FRotator  ControlRotation = GetControlRotation();
	const FRotator  YawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FRotationMatrix DirectionMatrix(YawRotation);

	if (FMath::IsNearlyEqual(MovementVector.Y, -1))
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardVector = DirectionMatrix.GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardVector, MovementVector.Y);
	}
	if (MovementVector.X != 0.f)
	{
		const FVector RightVector = DirectionMatrix.GetUnitAxis(EAxis::Y);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void AKnight::Look(const FInputActionValue &Value)
{
	const FVector2d LookVector = Value.Get<FVector2d>();
	float           DeltaPitch = LookVector.Y;
	float           DeltaYaw   = LookVector.X;
	if (CombatComponent && CombatComponent->IsAttacking())
	{
		constexpr float TurnCap = 1.15f;
		DeltaPitch              = FMath::Clamp(DeltaPitch, -TurnCap, TurnCap);
		DeltaYaw                = FMath::Clamp(DeltaYaw, -TurnCap, TurnCap);
	}
	AddControllerPitchInput(DeltaPitch);
	AddControllerYawInput(DeltaYaw);
}

void AKnight::Block(const FInputActionValue &Value)
{
	CombatComponent->ProcessInput(EAction::Block);
}

void AKnight::StopBlocking(const FInputActionValue &Value)
{
	if (CombatComponent->GetNextAction() == EAction::Block)
	{
		CombatComponent->ProcessInput(EAction::Idle);
	}
}

void AKnight::Slash(const FInputActionValue &Value)
{
	const bool bIsAltAttack = CombatComponent->IsAltAttack();
	EAction    NextAction   = (bIsAltAttack) ? EAction::LeftSlash : EAction::RightSlash;
	CombatComponent->ProcessInput(NextAction);
}

void AKnight::Overhead(const FInputActionValue &Value)
{
	const bool bIsAltAttack = CombatComponent->IsAltAttack();
	EAction    NextAction   = (bIsAltAttack) ? EAction::LeftOverhead : EAction::RightOverhead;
	CombatComponent->ProcessInput(NextAction);
}

void AKnight::Interact(const FInputActionValue &Value)
{
	if (CombatComponent->CanAct())
	{
		//draw a box trace to see if there's anything interactable here
		FVector    Start = Camera->GetComponentLocation();
		FVector    End   = Start + (125 * Camera->GetForwardVector());
		FVector    BoxSize(4.0f, 4.0f, 4.0f);
		FHitResult HitResult;
		UKismetSystemLibrary::BoxTraceSingle(this,
		                                     Start,
		                                     End,
		                                     BoxSize,
		                                     Camera->GetComponentRotation(),
		                                     TraceTypeQuery1,
		                                     true,
		                                     TArray<AActor *>(),
		                                     EDrawDebugTrace::ForDuration,
		                                     HitResult,
		                                     true);
		if (AActor *HitActor = HitResult.GetActor(); HitActor && HitActor->Implements<UInteractable>())
		{
			IInteractable::Execute_OnInteract(HitActor, this);
		}
	}
}

bool AKnight::CanBeSeenFrom(const FVector &ObserverLocation,
                            FVector &      OutSeenLocation,
                            int32 &        NumberOfLoSChecksPerformed,
                            float &        OutSightStrength,
                            const AActor * IgnoreActor,
                            const bool *   bWasVisible,
                            int32 *        UserData) const
{
	static const FName NAME_InTraceTag = "Skeleton Sight";
	FHitResult         HitResult;
	FVector            HeadLocation = GetMesh()->GetSocketLocation(FName("head"));

	bool bIsHeadObstructed = GetWorld()->LineTraceSingleByObjectType(HitResult,
	                                                                 ObserverLocation,
	                                                                 HeadLocation,
	                                                                 FCollisionObjectQueryParams(
		                                                                 ECC_TO_BITFIELD(ECC_WorldStatic) |
		                                                                 ECC_TO_BITFIELD(ECC_WorldDynamic)),
	                                                                 FCollisionQueryParams(
		                                                                 NAME_InTraceTag,
		                                                                 true,
		                                                                 IgnoreActor));

	++NumberOfLoSChecksPerformed;

	AActor *HitActor = HitResult.GetActor();
	if (bIsHeadObstructed == false && HitActor && HitActor->IsValidLowLevel() && HitActor->IsOwnedBy(this))
	{
		OutSeenLocation  = HeadLocation;
		OutSightStrength = 1.0f;

		return true;
	}

	bool bIsActorObstructed = GetWorld()->LineTraceSingleByObjectType(HitResult,
	                                                                  ObserverLocation,
	                                                                  GetActorLocation(),
	                                                                  FCollisionObjectQueryParams(
		                                                                  ECC_TO_BITFIELD(ECC_WorldStatic) |
		                                                                  ECC_TO_BITFIELD(ECC_WorldDynamic)),
	                                                                  FCollisionQueryParams(
		                                                                  NAME_InTraceTag,
		                                                                  true,
		                                                                  IgnoreActor));

	++NumberOfLoSChecksPerformed;
	HitActor = HitResult.GetActor();
	if (bIsActorObstructed == false && HitActor && HitActor->IsValidLowLevel() && HitActor->IsOwnedBy(this))
	{
		OutSeenLocation  = GetActorLocation();
		OutSightStrength = 1.0f;

		return true;
	}

	OutSightStrength = 0;
	return false;
}

// Called every frame
void AKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AKnight::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKnight::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKnight::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AKnight::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AKnight::StopBlocking);
		EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Triggered, this, &AKnight::Slash);
		EnhancedInputComponent->BindAction(OverheadAction, ETriggerEvent::Triggered, this, &AKnight::Overhead);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AKnight::Interact);
	}
}

bool AKnight::Injure(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData)
{
	const bool bDidTakeDamage = CombatComponent->TakeDamage(AttackInfo, AttackTraceHitData);
	if (bDidTakeDamage && HitCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShake, 1.0f);
	}
	if (CombatComponent->IsDead())
	{
		GetMesh()->bPauseAnims = true;
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->AddRadialImpulse(AttackTraceHitData.Location, 200.0f, 1000.0f, RIF_Constant, true);
		OnDeath();
	}
	KnightInjured.Broadcast();
	return bDidTakeDamage;
}

void AKnight::OnDeath()
{
	KnightDied.Broadcast();
	//TODO: Implement death logic
}

float AKnight::Heal(float Amount)
{
	return CombatComponent->Heal(Amount);
}

float AKnight::GetMaxHP()
{
	return CombatComponent->GetMaxHP();
}

float AKnight::GetCurrentHP()
{
	return CombatComponent->GetCurrentHP();
}

float AKnight::GetMaxStamina()
{
	return CombatComponent->GetMaxStamina();
}

float AKnight::GetCurrentStamina()
{
	return CombatComponent->GetCurrentStamina();
}

void AKnight::EquipWeapon(AWeapon *NewWeapon)
{
	CombatComponent->EquipWeapon(NewWeapon);
	NewWeapon->GetMesh()->AttachToComponent(GetMesh(),
	                                        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                        NewWeapon->GetAttackSocketName());
	if (UAnimSequenceBase *WeaponIdleAnimation = NewWeapon->GetIdleAnimation())
	{
		IdleAnimation = WeaponIdleAnimation;
	}
	KnightPickedUpWeapon.Broadcast(NewWeapon);
}
