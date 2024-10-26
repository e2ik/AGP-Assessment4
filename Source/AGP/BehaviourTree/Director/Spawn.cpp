#include "Spawn.h"
#include "AGP/GameMode/AIDirector.h"

USpawn::USpawn() {
    Name = "Spawn";
}

EStatus USpawn::Process() {
    if (Director) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Spawning enemies..."));
        Director->SetSpawnEnemies(true);
        Director->IncreaseNumOfEnemies();
    }
    return EStatus::SUCCESS;
}

void USpawn::Reset() {
    // Reset the spawn action
}

