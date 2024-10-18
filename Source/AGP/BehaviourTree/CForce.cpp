#include "CForce.h"

UCForce::UCForce() : UCNode() {
    // default to Always Succeed
    Type = EForceType::SUCCESS;
}

void UCForce::Initialize(FString InName) {
    Name = InName;
}

void UCForce::InitializeType(EForceType InType) {
    Type = InType;
}

EStatus UCForce::Process() {
    if (Children.Num() > 1) {
        UE_LOG(LogTemp, Warning, TEXT("Force node %s has more than one child"), *Name);
        return EStatus::FAILURE;
    }

    if (Children.Num() > 0) {
        EStatus ChildStatus = Children[0]->Process();

        switch (Type) {
            case EForceType::SUCCESS:
                return EStatus::SUCCESS;
            case EForceType::FAILURE:
                return EStatus::FAILURE;
            default:
                UE_LOG(LogTemp, Warning, TEXT("Unexpected Force Type"));
                return EStatus::SUCCESS;
        }
    }

    // force a success anyway
    return EStatus::SUCCESS;
}