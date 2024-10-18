#include "FollowPath.h"

UFollowPath::UFollowPath() : UCAction() {
    Name = "FollowPath";
}

EStatus UFollowPath::Process() {
    if (Owner) {
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            UE_LOG(LogTemp, Warning, TEXT("Following path..."));
            if (EnemyCharacter->StillPathing()) {
                return EStatus::RUNNING;
            } else {
                UE_LOG(LogTemp, Warning, TEXT("---Path Completed!!!"));
                return EStatus::SUCCESS;
            }
        }
    }
    return EStatus::FAILURE;
}

void UFollowPath::Reset() {
    // ...
}
