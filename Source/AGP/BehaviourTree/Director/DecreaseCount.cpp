#include "DecreaseCount.h"
#include "AGP/GameMode/AIDirector.h"

UDecreaseCount::UDecreaseCount() {
    Name = "Decrease Count";
}

EStatus UDecreaseCount::Process() {
    if (Director) {
        Director->DecreaseNumOfEnemies();
        Director->SetPlayerDeathCount(0);
    }
    return EStatus::SUCCESS;
}

void UDecreaseCount::Reset() {
    // Reset the spawn action
}

