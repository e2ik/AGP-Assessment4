#include "FireWeapon.h"
#include "AGP/Characters/EnemyCharacter.h"

UFireWeapon::UFireWeapon()
{
    Name = "FireWeapon";
}

EStatus UFireWeapon::Process()
{
    // actual action
    if (Owner) {
        // cast owner to enemycharacter
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            EnemyCharacter->FireWeapon();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Firing weapon..."));
    return EStatus::RUNNING;
}

void UFireWeapon::Reset()
{
    // reset fireweapon
}