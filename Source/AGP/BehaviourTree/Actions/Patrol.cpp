#include "Patrol.h"
#include "AGP/Characters/EnemyCharacter.h"

UPatrol::UPatrol() {
    Name = "Patrol";
}

EStatus UPatrol::Process() {
    // actual action
    if (Owner) {
        // cast owner to enemycharacter
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            EnemyCharacter->StartPatrol();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Patrolling..."));
    return EStatus::RUNNING;
}

void UPatrol::Reset() {
    // reset patrol
}
