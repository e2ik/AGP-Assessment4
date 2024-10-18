#include "CParallel.h"

UCParallel::UCParallel() : UCNode() {}

void UCParallel::Initialize(FString InName) {
    Name = InName;
}

EStatus UCParallel::Process() {
    bool bAnyRunning = false;
    for (UCNode* Child : Children) {
        EStatus ChildStatus = Child->Process();
        if (ChildStatus == EStatus::RUNNING) {
            bAnyRunning = true;
        } else if (ChildStatus == EStatus::FAILURE) {
            Reset();
            return EStatus::FAILURE;
        }
    }
    return bAnyRunning ? EStatus::RUNNING : EStatus::SUCCESS;
}