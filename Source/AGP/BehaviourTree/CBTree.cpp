#include "CBTree.h"

UCBTree::UCBTree() : UCNode() {}

void UCBTree::Initialize(FString InName) {
    Name = InName;
}

EStatus UCBTree::Process() {
    while (CurrentChild < Children.Num()) {
        CurrentStatus = Children[CurrentChild]->Process();
        if (CurrentStatus != EStatus::SUCCESS) {
            return CurrentStatus;
        }
        CurrentChild++;
    }
    return EStatus::SUCCESS;
}
