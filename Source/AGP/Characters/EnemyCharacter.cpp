// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"
#include "Algo/Reverse.h"
#include "HealthComponent.h"
#include "PlayerCharacter.h"
#include "AGP/ProceduralNodes/PathfindingSubsystem.h"
#include "AGP/BehaviourTree/BTComponent.h"
#include "AGP/BehaviourTree/AIAssignSubsystem.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");

	// Set up simple detection sphere
	DetectionSphere = CreateDefaultSubobject<USphereComponent>("Detection Sphere");
	DetectionSphere->InitSphereRadius(1000.0f);
	DetectionSphere->SetCollisionProfileName("Trigger");
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapBegin);

	StartingLocation = GetActorLocation();
	StoreLocation = FVector(MAX_FLT, MAX_FLT, MAX_FLT);
	SensedCharacter = nullptr;
	SensedWeapon = nullptr;
	bReplicates = true;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEnemyCharacter, EnemyType);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// DO NOTHING IF NOT ON THE SERVER
	if (GetLocalRole() != ROLE_Authority) return;
	
	PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>();
	AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();

	if (PawnSensingComponent) {
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSensedPawn);
	}

	// health check
	if (HealthComponent) {
		PreviousHealth = HealthComponent->GetCurrentHealth();
		HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::OnHealthChanged);
	}

	if (EnemyType == EEnemyType::UNASSIGNED) {
		EEnemyType RandomType = static_cast<EEnemyType>(FMath::RandRange(1, 3));
		SetEnemyType(RandomType);
		OnRep_EnemyType();
		UpdateEnemyMaterial();
	}

	// Check if AI Controller is assigned
	if (!GetController()) {
		AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, GetActorLocation(), GetActorRotation());
		if (AIController) { AIController->Possess(this); }
	}	
}

void AEnemyCharacter::OnRep_EnemyType()
{
	UpdateEnemyMaterial();
}

void AEnemyCharacter::MoveAlongPath()
{
	if (CurrentPath.IsEmpty()) return;
	
	FVector MovementDirection = CurrentPath[CurrentPath.Num()-1] - GetActorLocation();
	MovementDirection.Normalize();
	AddMovementInput(MovementDirection);
	if (FVector::Distance(GetActorLocation(), CurrentPath[CurrentPath.Num() - 1]) < PathfindingError) {
		CurrentPath.Pop();
	}
}

void AEnemyCharacter::TickPatrol()
{
	if (CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
	}
	// MoveAlongPath();
	FollowPath();
}

void AEnemyCharacter::TickEngage()
{
	
	if (!SensedCharacter.IsValid()) return;
	
	if (CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), SensedCharacter->GetActorLocation());
	}
	MoveAlongPath();

	// face player
	FVector DirectionToPlayer = SensedCharacter->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Z = 0;
	DirectionToPlayer.Normalize();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
	SetActorRotation(LookAtRotation);
	
	if (HasWeapon())
	{
		if (WeaponComponent->IsMagazineEmpty())
		{
			Reload();
		}
		Fire(SensedCharacter->GetActorLocation());
	}
}

void AEnemyCharacter::TickEvade()
{
	// Find the player and return if it can't find it.
	if (!SensedCharacter.IsValid()) return;

	if (CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPathAway(GetActorLocation(), SensedCharacter->GetActorLocation());
	}
	MoveAlongPath();
}

void AEnemyCharacter::OnSensedPawn(APawn* SensedActor)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(SensedActor))
	{
		SensedCharacter = TWeakObjectPtr<APlayerCharacter>(Player); 
		AIAssignSubsystem->NotifyPlayerSensed(true, this);
		if (!bHasSensedPlayer) {
			bHasSensedPlayer = true;
			ClearPath();
		}
	}
}

void AEnemyCharacter::UpdateSight()
{
	// if (!SensedCharacter.IsValid()) return;
	if (PawnSensingComponent)
	{
		if (!PawnSensingComponent->HasLineOfSightTo(SensedCharacter.Get()))
		{
			SensedCharacter = nullptr;
			AIAssignSubsystem->NotifyPlayerSensed(false, this);
			if (bHasSensedPlayer) {
				bHasSensedPlayer = false;
				ClearPath();
			}
		}
	}
}


// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DO NOTHING UNLESS IT IS ON THE SERVER
	if (GetLocalRole() != ROLE_Authority) return;

	if (!bIsShotAt) {
		if (PreviousHealth > HealthComponent->GetCurrentHealth()) {
			OnHealthChanged();
		}
	}

	// ShotAt check
	if (bIsShotAt)
	{
		ShotAtTimer += DeltaTime;
		if (ShotAtTimer >= 1.0f)
		{
			bIsShotAt = false;
			ShotAtTimer = 0.0f;
		}
		return;
	}

	if (HasWeapon()) {
		if (WeaponComponent->IsMagazineEmpty()) { Reload(); }
	}

	// Death Check
	if (HealthComponent->IsDead()) {
		AIAssignSubsystem->NotifyDeath(this);
		return;
	}

	if (bIsFiringWeapon) FireWeapon();
	if (bIsReloadingWeapon) ReloadWeapon();
	if (!SensedCharacter.IsValid()) bIsFiringWeapon = false;
	
	UpdateSight();

	if (bIsPatrolling) TickPatrol();
	if (bIsFollowingPath) FollowPath();
	if (bIsRepeatPath) {
		PatrolPath();
	} else { PathfindingSubsystem->ClearPatrolPath(); }


	// ! LAB FSM code: leaving this here just in case
	// switch(CurrentState)
	// {
	// case EEnemyState::Patrol:
	// 	TickPatrol();
	// 	if (SensedCharacter.IsValid())
	// 	{
	// 		if (HealthComponent->GetCurrentHealthPercentage() >= 0.4f)
	// 		{
	// 			CurrentState = EEnemyState::Engage;
	// 		} else
	// 		{
	// 			CurrentState = EEnemyState::Evade;
	// 		}
	// 		CurrentPath.Empty();
	// 	}
	// 	break;
	// case EEnemyState::Engage:
	// 	TickEngage();
	// 	if (HealthComponent->GetCurrentHealthPercentage() < 0.4f)
	// 	{
	// 		CurrentPath.Empty();
	// 		CurrentState = EEnemyState::Evade;
	// 	} else if (!SensedCharacter.IsValid())
	// 	{
	// 		CurrentState = EEnemyState::Patrol;
	// 	}
	// 	break;
	// case EEnemyState::Evade:
	// 	TickEvade();
	// 	if (HealthComponent->GetCurrentHealthPercentage() >= 0.4f)
	// 	{
	// 		CurrentPath.Empty();
	// 		CurrentState = EEnemyState::Engage;
	// 	} else if (!SensedCharacter.IsValid())
	// 	{
	// 		CurrentState = EEnemyState::Patrol;
	// 	}
	// 	break;
	// }

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) {
		if (AWeaponPickup* WeaponPickup = Cast<AWeaponPickup>(OtherActor)) {
			SensedWeapon = WeaponPickup;
		}
	}

	// TODO: Can add check for health pickups too
}

void AEnemyCharacter::FollowPath()
{
	if (CurrentPath.Num() == 0) {
		bIsFollowingPath = false;
		return;
	}

	if (PathIndex <= 0) {
		bIsFollowingPath = false;
		return;
	}

	if (bEndOfPath) {
		bEndOfPath = false;
		PathIndex = CurrentPath.Num() - 1;
		bIsFollowingPath = false;
		return;
	}

	if (bIsRepeatPath && !bEndOfPath) {
		if (PathIndex <= 0) {
			bEndOfPath = true;
		}
	}

    // Draw the current path
    // for (int32 i = 0; i < CurrentPath.Num() - 1; i++) {
    //     DrawDebugLine(GetWorld(), CurrentPath[i], CurrentPath[i + 1], FColor::Red, false, -1, 0, 2.0f );
    // }

    // Debug point for the current target
    // DrawDebugSphere(GetWorld(), CurrentPath[PathIndex], 20.0f, 4, FColor::Green, false, -1, 0, 2.0f );

	// Changing this to just check for XY as some nodes are lifted, hard to find a good PathfindingError value
	if (FVector::DistSquared2D(GetActorLocation(), CurrentPath[PathIndex]) <= FMath::Square(PathfindingError)) {
		PathIndex--;
		if (PathIndex <= 0) { CurrentPath.Empty(); }
	} else {
		FVector ActorLocation2D = GetActorLocation();
		ActorLocation2D.Z = 0.0f;		
		FVector TargetLocation2D = CurrentPath[PathIndex];
		TargetLocation2D.Z = 0.0f;		
		FVector Direction = (TargetLocation2D - ActorLocation2D).GetSafeNormal();

		if (bIsRepeatPath) { GetCharacterMovement()->MaxWalkSpeed = 299.0f; }
		else { GetCharacterMovement()->MaxWalkSpeed = 600.0f; }
		AddMovementInput(Direction);

		if (!bIsFiringWeapon) {
			FRotator TargetRotation = Direction.ToOrientationRotator();
			FRotator ConstrainRotation = FRotator(GetActorRotation().Pitch, TargetRotation.Yaw, GetActorRotation().Roll);
			FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), ConstrainRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
			SetActorRotation(SmoothRotation);
		}
	}

}

void AEnemyCharacter::GeneratePatrolPath()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (CurrentPath.Num() == 0) {
		CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
		PathIndex = CurrentPath.Num() - 1;
	}
}

void AEnemyCharacter::GeneratePathAway()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (SensedCharacter.IsValid()) {
		if (CurrentPath.Num() == 0) {
			CurrentPath = PathfindingSubsystem->GetPathAway(GetActorLocation(), SensedCharacter->GetActorLocation());
			PathIndex = CurrentPath.Num() - 1;
		}
	}
}

void AEnemyCharacter::GeneratePathToWeapon()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (SensedWeapon) {
		if (CurrentPath.Num() == 0) {
			CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), SensedWeapon->GetActorLocation());
			PathIndex = CurrentPath.Num() - 1;
		}
	}
}

void AEnemyCharacter::GeneratePathToPlayer()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (SensedCharacter.IsValid()) {
		if (CurrentPath.Num() == 0) {
			CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), SensedCharacter->GetActorLocation());
			PathIndex = CurrentPath.Num() - 1;
		}
	}
}

FVector AEnemyCharacter::GetPlayerLocation()
{
	if (SensedCharacter.IsValid()) {
		bHasStoredLocation = true;
		StoreLocation = SensedCharacter->GetActorLocation();
		StoreDirection = (StoreLocation - GetActorLocation()).GetSafeNormal();
		return SensedCharacter->GetActorLocation();
	}
	return StoreLocation;
}

void AEnemyCharacter::StartPatrol() {
	if (GetLocalRole() != ROLE_Authority) return;
	bIsPatrolling = true;
}

bool AEnemyCharacter::IsHealthBelowThreshold()
{
	return HealthComponent->GetCurrentHealthPercentage() < 0.4f;
}

void AEnemyCharacter::ClearPath()
{
	if (GetLocalRole() != ROLE_Authority) return;
	bIsPatrolling = false;
	bIsRepeatPath = false;
	PathIndex = 0;
	CurrentPath.Empty();
}

bool AEnemyCharacter::CanSeePlayer() { return SensedCharacter.IsValid(); }
bool AEnemyCharacter::CanSeeWeapon() { return SensedWeapon != nullptr; }
void AEnemyCharacter::StartFollowPath() { bIsFollowingPath = true; }
void AEnemyCharacter::StartRepeatPath() { bIsRepeatPath = true; }
bool AEnemyCharacter::StillPathing() { return bIsFollowingPath; }

void AEnemyCharacter::GeneratePathing()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (CurrentPath.Num() == 0) {
		PathIndex = 0;
		CurrentPath = PathfindingSubsystem->GetPatrolPath(GetActorLocation(), 3);
		Algo::Reverse(CurrentPath);
		PathIndex = CurrentPath.Num() - 1;
	} 
}

void AEnemyCharacter::PatrolPath()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (PathIndex == 0) ClearPatrolPath();
	if (PathIndex <= 0) {
		// Algo::Reverse(CurrentPath);
		PathIndex = CurrentPath.Num() - 1;
	}
	if (!bEndOfPath) FollowPath();
}

void AEnemyCharacter::ClearPatrolPath()
{
	if (GetLocalRole() != ROLE_Authority) return;
	PathfindingSubsystem->ClearPatrolPath();
}

void AEnemyCharacter::FireWeapon()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (bIsReloadingWeapon) bIsReloadingWeapon = false;
	bIsFiringWeapon = true;
	if (!SensedCharacter.IsValid()) return;
	FVector TargetLocation = SensedCharacter->GetActorLocation();
	Fire(TargetLocation);

	// ! Probably don't need this anymore
	FRotator TargetRotation = (TargetLocation - GetActorLocation()).ToOrientationRotator();
	FRotator ConstrainRotation = FRotator(GetActorRotation().Pitch, TargetRotation.Yaw, GetActorRotation().Roll);
	// FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), ConstrainRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
	SetActorRotation(ConstrainRotation);
}

void AEnemyCharacter::ReloadWeapon()
{
	if (GetLocalRole() != ROLE_Authority) return;
	bIsReloadingWeapon = true;
	Reload();
}

void AEnemyCharacter::GenerateFlankPath()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (CurrentPath.Num() == 0) {
		PathIndex = 0;
		PathfindingSubsystem->bIsFlanking = true;
		PathfindingSubsystem->bIsAddingNodes = true;
		PathfindingSubsystem->FlankDirection = StoreDirection;
		PathfindingSubsystem->FlankLocation = StoreLocation;
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), StoreLocation);
		PathIndex = CurrentPath.Num() - 1;
	}

	FVector CurrentLocation = GetActorLocation();
	FVector CurrentPlayerLocation = GetPlayerLocation();
	if (FVector::Dist(CurrentLocation, CurrentPlayerLocation) < 1000.0f) {
		// player is close
		AIAssignSubsystem->NotifyPlayerIsClose(true, this);
	}
}

bool AEnemyCharacter::HasValidFlankPath() { return CurrentPath.Num() > 0; }
bool AEnemyCharacter::IsDead() { return HealthComponent->IsDead(); }
void AEnemyCharacter::PlayDeathAnimation() { bIsDead = true; }

void AEnemyCharacter::OnHealthChanged()
{
	if (GetLocalRole() != ROLE_Authority) return;
	if (IsHealthBelowThreshold()) { bIsShotAt = false; }
	if (HealthComponent->GetCurrentHealth() < PreviousHealth && !SensedCharacter.IsValid() && !IsHealthBelowThreshold()) {
		bIsShotAt = true;
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Yaw += 90.0f;
		SetActorRotation(CurrentRotation);
	}
	PreviousHealth = HealthComponent->GetCurrentHealth();
}

bool AEnemyCharacter::IsMagazineEmpty()
{
	if (WeaponComponent) {
		return WeaponComponent->IsMagazineEmpty();
	}
	return false;
}

