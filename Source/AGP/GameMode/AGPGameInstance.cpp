 // Fill out your copyright notice in the Description page of Project Settings.
#include "AGPGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AGP/Pickups/WeaponPickup.h"

UClass* UAGPGameInstance::GetWeaponPickupClass() const
{
    return WeaponPickupClass.Get();
}

void UAGPGameInstance::SpawnGroundHitParticle(const FVector& SpawnLocation)
{
    if (GroundHitParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GroundHitParticle, SpawnLocation);
    }
}

void UAGPGameInstance::SpawnCharacterHitParticle(const FVector& SpawnLocation)
{
    if (CharacterHitParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CharacterHitParticle, SpawnLocation);
    }
}

void UAGPGameInstance::PlayGunshotSoundAtLocation(const FVector& Location)
{
    if (GunshotSoundCue) {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunshotSoundCue, Location, FRotator::ZeroRotator);
    }
}

void UAGPGameInstance::PlayGunshotSound2D()
{
    if (GunshotSoundCue) {
        UGameplayStatics::PlaySound2D(GetWorld(), GunshotSoundCue);
    }
}


