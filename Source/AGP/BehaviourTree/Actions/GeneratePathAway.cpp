#include "GeneratePathAway.h"

UGeneratePathAway::UGeneratePathAway() : UCAction() {
    Name = "GeneratePathAway";
}

EStatus UGeneratePathAway::Process() {
    if (Owner) {
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            UE_LOG(LogTemp, Log, TEXT("Pathing Away......"));
            EnemyCharacter->GeneratePathAway();
            EnemyCharacter->StartFollowPath();
            return EStatus::SUCCESS;
        }
    }
    return EStatus::FAILURE;
}

void UGeneratePathAway::Reset() {
    // ...
}