#include "SpawnWeapon.h"
#include "AGP/GameMode/AIDirector.h"

USpawnWeapon::USpawnWeapon() {
    Name = "Decrease Count";
}

EStatus USpawnWeapon::Process() {
    if (Director) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Spawning weapon..."));
        Director->SpawnWeaponAtPlayer();
        Director->TotalDeathResets = 0;
        Director->bTimerCheck = false;
    }
    return EStatus::SUCCESS;
}

void USpawnWeapon::Reset() {
    // Reset the spawn action
}

