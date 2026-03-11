// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\CombatComponent.h"

#include "AnimationTransition.h"
#include "AttackInfo.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"
#include "Weapon.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these
	// features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick  = false;
	DefaultWeapon					   = nullptr;
	MaxHP							   = 100.0f;
	CurrentHP						   = MaxHP;
	MaxStamina						   = 100.0f;
	CurrentStamina					   = MaxStamina;
	StaminaRegenerationRate			   = 2.0f;
	bCanRegenerateStamina			   = true;
	bIsParrying						   = false;
	bIsBlocking						   = false;
	bCanAct							   = true;
	bIsDead							   = false;
	bIsAltAttack					   = false;
	bIsAttacking					   = false;
	Weapon							   = nullptr;
	WeaponAttackDataTable			   = nullptr;
	WeaponAnimationTransitionDataTable = nullptr;
	CurrentAction					   = EAction::Idle;
	NextAction						   = EAction::Idle;
	TracePrecision					   = 10;
	bAutoDetermineTracePrecision	   = true;
	bDrawDebugTraces				   = true;
	TraceBoxHalfSize				   = 2.5f;
	HitSound						   = nullptr;
	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{

	Super::BeginPlay();
	// ...
	if (DefaultWeapon)
	{
		const FVector  DefaultPosition = FVector(0);
		const FRotator DefaultRotation = FRotator(0);
		AActor		  *Actor		   = GetWorld()->SpawnActor(DefaultWeapon, &DefaultPosition, &DefaultRotation);
		if (AWeapon *TempWeapon = Cast<AWeapon>(Actor))
		{
			Weapon = TempWeapon;
		}
		else
		{
			UE_LOGFMT(LogTemp, Error, "/!\\ Default class supplied for weapon does not derive from AWeapon /!\\");
			Actor->Destroy();
		}
	}
	if (Weapon)
	{
		const ACharacter *Owner = Cast<ACharacter>(GetOwner());
		Weapon->GetMesh()->AttachToComponent(
			Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r_socket"));
		Weapon->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SetWeaponAttackDataTable(Weapon->GetAttackDataTable());
		SetWeaponAnimationDataTable(Weapon->GetAnimationDataTable());
		LoadAnimDataTable();
		if (bAutoDetermineTracePrecision)
		{
			CalculateTracePrecision();
		}
		IgnoreActors.Add(Weapon);
	}
	CurrentHP	   = MaxHP;
	CurrentStamina = MaxStamina;
	GetWorld()->GetTimerManager().SetTimer(
		StaminaRegenTimerHandle, this, &UCombatComponent::RegenerateStamina, 0.1f, true);
	IgnoreActors.Add(GetOwner());
	if (IGenericTeamAgentInterface *OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetOwner()))
	{
		OwnerTeamId = OwnerTeamAgentInterface->GetGenericTeamId();
	}
}

void UCombatComponent::RunWeaponTrace()
{
	if (!Weapon)
	{
		return;
	}
	TArray<FHitResult>			 HitActors;
	TMap<AActor *, FHitResult *> HitResultsCorrected;
	const FVector				 WeaponStartTracePoint = Weapon->GetStartTracePoint()->GetComponentLocation();
	const FVector				 WeaponEndTracePoint   = Weapon->GetEndTracePoint()->GetComponentLocation();
	if (PreviousTracePoints.IsEmpty())
	{
		for (int32 Index = 0; Index < TracePrecision; ++Index)
		{
			PreviousTracePoints.Add(Index,
									CalculateIntermediateTracePoint(Index, WeaponStartTracePoint, WeaponEndTracePoint));
		}
	}

	for (int32 Index = 0; Index < TracePrecision; ++Index)
	{
		const FVector  Start	= PreviousTracePoints[Index];
		const FVector  End		= CalculateIntermediateTracePoint(Index, WeaponStartTracePoint, WeaponEndTracePoint);
		const FVector  HalfSize = FVector(TraceBoxHalfSize);
		const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		const EDrawDebugTrace::Type DebugTraceType =
			bDrawDebugTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		UKismetSystemLibrary::BoxTraceMulti(
			this, Start, End, HalfSize, Rotation, TraceTypeQuery1, true, IgnoreActors, DebugTraceType, HitActors, true);

		PreviousTracePoints[Index] = End;
		for (auto &HitResult : HitActors)
		{
			if (HitResult.GetActor())
			{
				HitResultsCorrected.Add(HitResult.GetActor(), &HitResult);
			}
		}
		for (auto [HitActor, HitResult] : HitResultsCorrected)
		{
			if (IDamageable *DamageableActor = Cast<IDamageable>(HitActor))
			{
				if (DamageableActor->IsFriendly(OwnerTeamId))
				{
					continue;
				}

				const FAttackInfo *AttackInfo =
					WeaponAttackDataTable->FindRow<FAttackInfo>(UEnum::GetValueAsName(CurrentAction), "");
				if (AttackInfo)
				{
					DamageableActor->Injure(*AttackInfo, *HitResult);
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(
							-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Damaged at trace index: %d"), Index));
					}
				}
				else
				{
					UE_LOGFMT(LogTemp,
							  Error,
							  "/!\\ Invalid Attack Info Supplied on attack: {0} /!\\",
							  UEnum::GetValueAsName(CurrentAction));
				}
			}
			IgnoreActors.AddUnique(HitActor);
		}
		HitActors.Empty();
		HitResultsCorrected.Empty();
	}
}

void UCombatComponent::EquipWeapon(AWeapon *NewWeapon)
{
	if (NewWeapon == nullptr || NewWeapon->IsValidLowLevelFast() == false)
	{
		UE_LOG(LogCore, Error, TEXT("/!\\ Invalid Weapon equip attempt!"));
	}
	if (Weapon)
	{
		Weapon->Destroy();
	}
	this->Weapon = NewWeapon;
	UE_LOGFMT(LogCore,
			  Display,
			  "Equipping weapon {0} with attack data table {1} and anim data table {2}",
			  NewWeapon->GetName(),
			  NewWeapon->GetAttackDataTable()->GetName(),
			  NewWeapon->GetAnimationDataTable()->GetName());
	SetWeaponAttackDataTable(NewWeapon->GetAttackDataTable());
	SetWeaponAnimationDataTable(NewWeapon->GetAnimationDataTable());
	LoadAnimDataTable();
	if (bAutoDetermineTracePrecision)
	{
		CalculateTracePrecision();
	}
	ResetTraceState();
}

void UCombatComponent::LoadAnimDataTable()
{
	if (WeaponAnimationTransitionDataTable)
	{
		ActionTransitions.Empty();
		TArray<FAnimationTransition *> Transitions;
		WeaponAnimationTransitionDataTable->GetAllRows("", Transitions);
		for (const auto &Transition : Transitions)
		{
			if (ActionTransitions.Contains(Transition->FromState))
			{
				ActionTransitions[Transition->FromState].Add({Transition->ToState, Transition->MontageSectionName});
			}
			else
			{
				ActionTransitions.Add(Transition->FromState, {{Transition->ToState, Transition->MontageSectionName}});
			}
			UE_LOGFMT(LogCore,
					  Display,
					  "Loaded transition from {From} to {To} with section name: {SectionName}",
					  ("From", UEnum::GetValueAsString(Transition->FromState)),
					  ("To", UEnum::GetValueAsString(Transition->ToState)),
					  ("SectionName", Transition->MontageSectionName));
		}
	}
}

void UCombatComponent::RegenerateStamina()
{
	if (bCanRegenerateStamina)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenerationRate, 0.0f, MaxStamina);
	}
}

FVector UCombatComponent::CalculateIntermediateTracePoint(const int32	 TracePointIndex,
														  const FVector &TraceStart,
														  const FVector &TraceEnd) const
{
	return TraceStart + TracePointIndex * ((TraceEnd - TraceStart) / TracePrecision);
}

void UCombatComponent::CalculateTracePrecision()
{
	if (!Weapon)
	{
		UE_LOGFMT(LogTemp, Error, "/!\\ Trace precision cannot be determined, weapon is null /!\\");
		return;
	}

	const FVector TraceStart = Weapon->GetStartTracePoint()->GetComponentLocation();
	const FVector TraceEnd	 = Weapon->GetEndTracePoint()->GetComponentLocation();
	TracePrecision = FMath::CeilToInt32((FVector::Distance(TraceStart, TraceEnd)) / (TraceBoxHalfSize * 2.0f));
}


// Called every frame
void UCombatComponent::TickComponent(float						  DeltaTime,
									 ELevelTick					  TickType,
									 FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

bool UCombatComponent::TakeDamage(const FAttackInfo &AttackInfo, const FHitResult &AttackTraceHitData)
{
	if (bIsDead)
	{
		return false;
	}

	UAnimInstance *AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
	if (AttackInfo.bIsParryable && bIsParrying)
	{
		if (USoundBase *ClashSound = Weapon->GetClashSound())
		{
			UGameplayStatics::SpawnSoundAttached(ClashSound, GetOwner()->GetRootComponent());
		}
		AnimInstance->Montage_Play(Weapon->GetWeaponAnimMontage());
		AnimInstance->Montage_JumpToSection(FName("block_hit"));
		SubtractStaminaCost(-AttackInfo.BlockStaminaDamage / 1.5f);
		return false; // no damage taken on a parry
	}

	float DamageAmount = AttackInfo.DamageAmount;
	if (AttackInfo.bIsBlockable && bIsBlocking)
	{
		if (USoundBase *ClashSound = Weapon->GetClashSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, ClashSound, AttackTraceHitData.Location, FRotator(0.0f));
		}
		AnimInstance->Montage_Play(Weapon->GetWeaponAnimMontage());
		AnimInstance->Montage_JumpToSection(FName("block_hit"));
		if (CurrentStamina == 0)
		{
			DamageAmount /= 2.0f;
		}
		else
		{
			SubtractStaminaCost(AttackInfo.StaminaCost);
			return false;
		}
	}

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,
											  HitSound,
											  GetOwner()->GetActorLocation(),
											  FRotator(0.0f),
											  1.0f,
											  1.0f,
											  0.0f,
											  nullptr,
											  nullptr,
											  GetOwner());
	}

	if (FMath::IsNearlyEqual(CurrentHP, 0.0f))
	{
		bIsDead = true;
	}

	return true;
}

void UCombatComponent::SubtractStaminaCost(const float StaminaCost)
{
	CurrentStamina = FMath::Clamp(CurrentStamina - StaminaCost, 0.0f, MaxStamina);
}

float UCombatComponent::Heal(float Amount)
{
	if (!bIsDead)
	{
		CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, MaxHP);
	}
	return CurrentHP;
}

void UCombatComponent::ResetTraceState()
{
	PreviousTracePoints.Empty();
	IgnoreActors.Empty();
	IgnoreActors.Add(Weapon);
	IgnoreActors.Add(GetOwner());
}

void UCombatComponent::ProcessInput(EAction Action)
{
	NextAction = (CurrentStamina == 0) ? EAction::Idle : Action;
	if (bCanAct)
	{
		PerformQueuedAction();
	}
}

void UCombatComponent::PerformQueuedAction()
{
	if (CurrentAction == EAction::Idle && NextAction == EAction::Idle)
	{
		return;
	}

	if (ActionTransitions.Contains(CurrentAction) && ActionTransitions[CurrentAction].Contains(NextAction))
	{
		UAnimInstance *AnimInstance	  = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
		const FName	  &MontageSection = ActionTransitions[CurrentAction][NextAction];
		if (AnimInstance)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(1,
												 2.0f,
												 FColor::Emerald,
												 FString::Printf(TEXT("CurrentAction %s, Next Action %s"),
																 *UEnum::GetValueAsString(CurrentAction),
																 *UEnum::GetValueAsString(NextAction)));
			}
			if (NextAction != EAction::Idle)
			{
				AnimInstance->StopAllMontages(0.0f);
				AnimInstance->Montage_Play(Weapon->GetWeaponAnimMontage());
				AnimInstance->Montage_JumpToSection(MontageSection);
				GetWorld()->GetTimerManager().PauseTimer(StaminaRegenTimerHandle);
			}
			CurrentAction = NextAction;
			if (CurrentAction == EAction::Idle)
			{
				bIsAltAttack = false;
				GetWorld()->GetTimerManager().UnPauseTimer(StaminaRegenTimerHandle);
			}
			else if (CurrentAction == EAction::Block)
			{
				bIsAltAttack = true;
			}
			else
			{
				bIsAltAttack = !bIsAltAttack;
				NextAction	 = EAction::Idle;
				if (const FAttackInfo *AttackInfo =
						WeaponAttackDataTable->FindRow<FAttackInfo>(UEnum::GetValueAsName(CurrentAction), ""))
				{
					SubtractStaminaCost(AttackInfo->StaminaCost);
				}
			}
		}
	}
	else
	{
		UE_LOGFMT(LogCore,
				  Warning,
				  "Invalid action supplied to the combat component. Transition from {Current} to {Next} does not exist",
				  ("Current", UEnum::GetValueAsString(CurrentAction)),
				  ("Next", UEnum::GetValueAsString(NextAction)));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1,
											 2.0f,
											 FColor::Red,
											 FString::Printf(TEXT("Invalid action transition from:  %s to %s"),
															 *UEnum::GetValueAsString(CurrentAction),
															 *UEnum::GetValueAsString(NextAction)));
		}
	}
}
