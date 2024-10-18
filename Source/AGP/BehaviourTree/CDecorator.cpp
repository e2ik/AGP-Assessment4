#include "CDecorator.h"

UCDecorator::UCDecorator() : UCNode() {
    Condition = nullptr;
}

void UCDecorator::Initialize(FString InName) {
    Name = InName;
}

void UCDecorator::InitializeCondition(ConditionFunction InCondition) {
    Condition = InCondition;
}

void UCDecorator::SetOwner(AActor* InOwner) {
    Owner = InOwner;
}

EStatus UCDecorator::Process() {

    if (Children.Num() != 1) {
        UE_LOG(LogTemp, Warning, TEXT("Decorator %s has %d children. It should have 1."), *Name, Children.Num());
        return EStatus::FAILURE;
    }

    if (Condition()) {
        return Children[0]->Process();
    } else {
        return EStatus::FAILURE;
    }

}
