#include "CAction.h"
#include "AGP/Characters//EnemyCharacter.h"

UCAction::UCAction() {
    Name = "Action";
}

void UCAction::Initialize(FString InName) {
    Name = InName;
}

EStatus UCAction::Process() {
    return EStatus::SUCCESS;
}

void UCAction::Reset() {
    // reset action
}

void UCAction::SetOwner(AActor* InOwner) {
    Owner = InOwner;
}