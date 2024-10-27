// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupManagerSubsystem.h"

#include "WeaponPickup.h"
#include "AGP/GameMode/AGPGameInstance.h"
#include "AGP/ProceduralNodes/PathfindingSubsystem.h"

void UPickupManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// We don't want this pickup manager to do any spawning if it isn't
	// on the server.
	// A value < NM_Client is any type of server. So if it is >=
	// to NM_Client or == NM_Client then we know it is the client
	// and we don't want to spawn.
	if (GetWorld()->GetNetMode() >= NM_Client)
	{
		return;
	}

	if (PossibleSpawnLocations.IsEmpty())
	{
		PopulateSpawnLocations();
	}

	TimeSinceLastSpawn += DeltaTime;
	if (TimeSinceLastSpawn >= PickupSpawnRate)
	{
		SpawnWeaponPickup();
		TimeSinceLastSpawn = 0.0f;
	}
}

void UPickupManagerSubsystem::SpawnWeaponPickup()
{
    if (PossibleSpawnLocations.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Unable to spawn weapon pickup."));
        return;
    }

    FVector SpawnPosition = PossibleSpawnLocations[FMath::RandRange(0, PossibleSpawnLocations.Num() - 1)];
    FVector NewPosition = SpawnPosition;

    if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
        if (SpawnedPickups.Contains(NewPosition)) {
            AWeaponPickup* ExistingPickup = SpawnedPickups[NewPosition];
            if (IsValid(ExistingPickup)) { ExistingPickup->Destroy(); }
        }

        AWeaponPickup* NewPickup = GetWorld()->SpawnActor<AWeaponPickup>(GameInstance->GetWeaponPickupClass(), NewPosition, FRotator::ZeroRotator);
        if (NewPickup) { SpawnedPickups.Add(NewPosition, NewPickup); }
    }
}

void UPickupManagerSubsystem::SpawnWeaponPickupNearPlayer(const FVector& PlayerLocation, int32 Level)
{
    if (PossibleSpawnLocations.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Unable to spawn weapon pickup."));
        return;
    }

    FVector ClosestSpawnPosition;
    float ClosestDistance = FLT_MAX;

    for (const FVector& SpawnPosition : PossibleSpawnLocations) {
        float Distance = FVector::Dist(PlayerLocation, SpawnPosition);

        if (Distance < ClosestDistance) {
            ClosestDistance = Distance;
            ClosestSpawnPosition = SpawnPosition;
        }
    }

    if (ClosestDistance < FLT_MAX) {
        FVector NewPosition = ClosestSpawnPosition;

        if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
            if (SpawnedPickups.Contains(NewPosition)) {
                AWeaponPickup* ExistingPickup = SpawnedPickups[NewPosition];
                if (IsValid(ExistingPickup)) { ExistingPickup->Destroy(); }
            }

            AWeaponPickup* NewPickup = GetWorld()->SpawnActor<AWeaponPickup>(GameInstance->GetWeaponPickupClass(), NewPosition, FRotator::ZeroRotator);
            if (NewPickup) {
				NewPickup->SetWeaponStats(Level);
				NewPickup->UpdateWeaponPickupMaterial();
				SpawnedPickups.Add(NewPosition, NewPickup);
				}
        }
    }
}



void UPickupManagerSubsystem::PopulateSpawnLocations()
{
	PossibleSpawnLocations.Empty();
	if (const UPathfindingSubsystem* PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>())
	{
		PossibleSpawnLocations = PathfindingSubsystem->GetWaypointPositions();
	}
}

void UPickupManagerSubsystem::RemovePickup(const FVector& Location)
{
	if (SpawnedPickups.Contains(Location)) {
		AWeaponPickup* PickupToRemove = SpawnedPickups[Location];
		if (IsValid(PickupToRemove)){
			PickupToRemove->Destroy();
			SpawnedPickups.Remove(Location);
		}
	}
}
