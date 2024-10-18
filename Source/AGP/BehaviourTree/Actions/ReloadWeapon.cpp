#include "ReloadWeapon.h"

UReloadWeapon::UReloadWeapon()
{
    Name = "ReloadWeapon";
}

EStatus UReloadWeapon::Process()
{
    // actual action
    if (Owner) {
        // cast owner to enemycharacter
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner);
        if (EnemyCharacter) {
            EnemyCharacter->ReloadWeapon();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Reloading weapon..."));
    return EStatus::RUNNING;
}

void UReloadWeapon::Reset()
{
    // reset weaponreload
}