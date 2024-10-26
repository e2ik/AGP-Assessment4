#include "IncreaseCount.h"
#include "AGP/GameMode/AIDirector.h"

UIncreaseCount::UIncreaseCount() {
    Name = "Increase Count";
}

EStatus UIncreaseCount::Process() {
    if (Director) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Increasing enemy count..."));
        Director->IncreaseNumOfEnemies();
    }
    return EStatus::SUCCESS;
}

void UIncreaseCount::Reset() {
    // Reset the spawn action
}

