#include "GeneratePath.h"

UGeneratePath::UGeneratePath() : UCAction() {
    Name = "GeneratePath";
    Type = EPathType::RANDOM;
}

void UGeneratePath::InitializeType(EPathType InType) {
    Type = InType;
}

EStatus UGeneratePath::Process() {
    UE_LOG(LogTemp, Warning, TEXT("GeneratePath Process"));
    if (Owner) {
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            switch (Type) {
                case EPathType::PLAYER:
                    UE_LOG(LogTemp, Log, TEXT("Pathing to Player"));
                    EnemyCharacter->GeneratePathToPlayer();
                    EnemyCharacter->StartFollowPath();
                    return EStatus::SUCCESS;
                    break;
                case EPathType::WEAPON:
                    // fix?                    
                    if (EnemyCharacter->HasWeapon()) {
                        UE_LOG(LogTemp, Log, TEXT("Tried to Path to Weapon, Has Weapon Already! Force Success"));
                        return EStatus::SUCCESS;
                    } else {
                        UE_LOG(LogTemp, Log, TEXT("Pathing to Weapon"));
                        EnemyCharacter->GeneratePathToWeapon();
                        EnemyCharacter->StartFollowPath();
                        return EStatus::SUCCESS;
                    }
                    break;
                case EPathType::RANDOM:
                    UE_LOG(LogTemp, Log, TEXT("Pathing Randomly"));
                    EnemyCharacter->GeneratePatrolPath();
                    EnemyCharacter->StartFollowPath();
                    return EStatus::SUCCESS;
                    break;
                case EPathType::STATIC:
                    UE_LOG(LogTemp, Log, TEXT("Pathing Static Route"));
                    EnemyCharacter->GeneratePathing();
                    EnemyCharacter->StartRepeatPath();
                    return EStatus::SUCCESS;
                    break;
                case EPathType::FLANK:
                    UE_LOG(LogTemp, Log, TEXT("Pathing to Flank"));
                    EnemyCharacter->GenerateFlankPath();
                    EnemyCharacter->StartFollowPath();
                    return EStatus::SUCCESS;
                    break;
            }            
        }
    }
    return EStatus::FAILURE;
}

void UGeneratePath::Reset() {
    // ...
}