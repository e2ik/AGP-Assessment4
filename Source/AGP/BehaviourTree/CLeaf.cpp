#include "CLeaf.h"

UCLeaf::UCLeaf() : UCNode() {}

void UCLeaf::Initialize(FString InName, UCAction* InAction) {
    Name = InName;
    Action = InAction;
}

EStatus UCLeaf::Process() {
    // condition or action
    return Action->Process();
}

void UCLeaf::Reset() {
    // reset action
    Action->Reset();
}