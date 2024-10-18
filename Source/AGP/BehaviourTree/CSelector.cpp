#include "CSelector.h"

UCSelector::UCSelector() : UCNode() {}

void UCSelector::Initialize(FString InName) {
    Name = InName;
}

EStatus UCSelector::Process() {
    for (int32 i = 0; i < Children.Num(); i++) {
        EStatus ChildStatus = Children[i]->Process();
        if (ChildStatus == EStatus::SUCCESS) {
            return EStatus::SUCCESS;
        } else if (ChildStatus == EStatus::RUNNING) {
            return EStatus::RUNNING;
        }
    }
    return EStatus::FAILURE;
}