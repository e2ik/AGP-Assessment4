// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "EnemyCharacter.generated.h"

class UPawnSensingComponent;
class APlayerCharacter;
class UPathfindingSubsystem;
class UBTComponent;
class UAIAssignSubsystem;


UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	UNASSIGNED,
	STATIONARY,
	ROAMING,
	PATHING
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol,
	Engage,
	Evade
};

UCLASS()
class AGP_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Behaviour Tree Additions
	UFUNCTION(BlueprintCallable)
	EEnemyType GetEnemyType() { return EnemyType; }
	UFUNCTION(BlueprintCallable)
	void SetEnemyType(EEnemyType Type) { EnemyType = Type; }
	UPROPERTY(VisibleAnywhere)
	UBTComponent* BTComponent = nullptr;
	UPROPERTY()
	FTimerHandle LostSightTimerHandle;

	bool bIsPatrolling = false;
	bool bIsFollowingPath = false;
	bool bIsRepeatPath = false;
	bool CanSeePlayer();
	bool CanSeeWeapon();
	void StartPatrol();
	void ClearPath();
	void StartFollowPath();
	void StartRepeatPath();
	void GeneratePatrolPath();
	void GeneratePathAway();
	void GeneratePathToWeapon();
	void GeneratePathToPlayer();
	void GenerateFlankPath();
	bool StillPathing();
	void ClearPatrolPath();
	bool HasValidFlankPath();
	void GeneratePathing();
	bool IsMagazineEmpty();

	UPROPERTY()
	bool bIsFiringWeapon = false;
	bool bIsReloadingWeapon = false;
	bool IsHealthBelowThreshold();
	bool IsDead();

	UFUNCTION()
	void FireWeapon();
	UFUNCTION()
	void ReloadWeapon();
	UFUNCTION()
	void PlayDeathAnimation();
	UFUNCTION()
	FVector GetPlayerLocation();

	UPROPERTY()
	bool bHasStoredLocation = false;
	UPROPERTY()
	FVector StoreLocation;
	UPROPERTY()
	FVector StartingLocation;
	UPROPERTY()
	FVector StoreDirection;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnemyMaterial();

	// end Behaviour Tree Additions

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
    void OnRep_EnemyType();

	// Behaviour Tree Additions
	UPROPERTY(ReplicatedUsing = OnRep_EnemyType, EditAnywhere)
	EEnemyType EnemyType = EEnemyType::UNASSIGNED;
	UPROPERTY()
	UAIAssignSubsystem* AIAssignSubsystem;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* DetectionSphere;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(VisibleAnywhere)
	AWeaponPickup* SensedWeapon;
	UPROPERTY()
	bool bIsDead = false;
	UFUNCTION(BlueprintCallable)
	bool IsPlayerDead() { return bIsDead; }
	// end Behaviour Tree Additions

	void MoveAlongPath();
	void TickPatrol();
	void TickEngage();
	void TickEvade();

	UFUNCTION()
	void OnSensedPawn(APawn* SensedActor);
	void UpdateSight();


	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerCharacter> SensedCharacter;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> CurrentPath;

	UPROPERTY(EditAnywhere)
	EEnemyState CurrentState = EEnemyState::Patrol;

	UPROPERTY(EditAnywhere)
	float PathfindingError = 50.0f;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	void FollowPath();
	void PatrolPath();

	UPROPERTY()
	bool bHasSensedPlayer = false;
	UPROPERTY()
	bool bEndOfPath = false;

	UFUNCTION()
	void OnHealthChanged();

	UPROPERTY()
	float PreviousHealth;

	bool bIsShotAt;
	float ShotAtTimer = 0.0f;

	int32 PathIndex = 0;

};
