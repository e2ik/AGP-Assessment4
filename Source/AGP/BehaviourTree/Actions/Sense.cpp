#include "Sense.h"
#include "AGP/Characters/EnemyCharacter.h"

USense::USense() {
    Name = "Sense";
    Type = ESenseObject::PLAYER;
}

void USense::InitializeType(ESenseObject InType) {
    Type = InType;
}

EStatus USense::Process() {
    // actual action
    if (Owner) {
        // cast owner to enemycharacter
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);

        if (Type == ESenseObject::PLAYER) {
            if (EnemyCharacter) {
                if (EnemyCharacter->CanSeePlayer()) {
                    return EStatus::SUCCESS;
                } else {
                    return EStatus::FAILURE;
                }
            }
        }
        if (Type == ESenseObject::WEAPON) {
            if (EnemyCharacter) {
                if (EnemyCharacter->CanSeeWeapon()) {
                    return EStatus::SUCCESS;
                } else {
                    return EStatus::FAILURE;
                }
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Sensing..."));
    return EStatus::RUNNING;
}

void USense::Reset() {
    // ...
}