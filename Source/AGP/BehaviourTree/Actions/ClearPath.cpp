#include "ClearPath.h"

UClearPath::UClearPath() {
    Name = "ClearPath";
}

EStatus UClearPath::Process() {
    // actual action
    if (Owner) {
        // cast owner to enemycharacter
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            EnemyCharacter->ClearPath();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Clearing path..."));
    return EStatus::SUCCESS;
}

void UClearPath::Reset() {
    // reset clearpath
}
