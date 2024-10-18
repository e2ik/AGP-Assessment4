#include "CInverter.h"

UCInverter::UCInverter() : UCNode() {}

void UCInverter::Initialize(FString InName) {
    Name = InName;
}

EStatus UCInverter::Process() {
    if (Children.Num() != 1) {
        UE_LOG(LogTemp, Warning, TEXT("Inverter %s has %d children. It should have 1."), *Name, Children.Num());
        return EStatus::FAILURE;
    }

    EStatus ChildStatus = Children[0]->Process();

    switch (ChildStatus) {
        case EStatus::SUCCESS:
            return EStatus::FAILURE;
        case EStatus::FAILURE:
            return EStatus::SUCCESS;
        case EStatus::RUNNING:
        default:
            return EStatus::RUNNING;
    }
    // no child nodes
    return EStatus::FAILURE;
}