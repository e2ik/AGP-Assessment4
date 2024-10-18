#include "CSequence.h"

UCSequence::UCSequence() : UCNode() {}

void UCSequence::Initialize(FString InName) {
    Name = InName;
}

EStatus UCSequence::Process()
{
    // log the name of the sequence white text
    UE_LOG(LogTemp, Log, TEXT("Sequence: %s"), *Name);

    if (CurrentChild < Children.Num()) {
        EStatus ChildStatus = Children[CurrentChild]->Process();

        switch (ChildStatus)        {
            case EStatus::RUNNING:
                UE_LOG(LogTemp, Warning, TEXT("RUNNING: %s"), *Children[CurrentChild]->Name);
                return EStatus::RUNNING;

            case EStatus::FAILURE:
                UE_LOG(LogTemp, Warning, TEXT("FAILURE: %s"), *Children[CurrentChild]->Name);
                Reset();
                return EStatus::FAILURE;

            case EStatus::SUCCESS:
                CurrentChild++;
                
                if (CurrentChild >= Children.Num()) {
                    Reset();
                    return EStatus::SUCCESS;
                } else {
                    UE_LOG(LogTemp, Warning, TEXT("SUCCESS: %s"), *Children[CurrentChild]->Name);
                    return EStatus::RUNNING;
                }

            default:
                UE_LOG(LogTemp, Warning, TEXT("Unexpected Status Current child: %d"), CurrentChild);
                Reset();
                return EStatus::FAILURE;
        }
    } else {
        Reset();
        return EStatus::SUCCESS;
    }
}
