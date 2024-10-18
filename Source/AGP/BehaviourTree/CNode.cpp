#include "CNode.h"

UCNode::UCNode() {
    Status = EStatus::IDLE;
    Name = "Node";
    CurrentChild = 0;
}

void UCNode::Initialize(FString InName) {
    Name = InName;
}

EStatus UCNode::GetNodeStatus() const {
    return Status;
}

void UCNode::SetNodeStatus(EStatus NewStatus) {
    Status = NewStatus;
}

void UCNode::AddChild(UCNode* Child) {
    Children.Add(Child);
}

EStatus UCNode::Process() {
    return Children[CurrentChild]->Process();
}

void UCNode::Reset() {
    CurrentChild = 0;
    for (UCNode* Child : Children) {
        Child->Reset();
    }
}

UCNode* UCNode::GetChild(int32 Index) {
    return Children[Index];
}