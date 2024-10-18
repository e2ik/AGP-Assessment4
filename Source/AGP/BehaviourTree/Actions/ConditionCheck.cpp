#include "ConditionCheck.h"

UConditionCheck::UConditionCheck() {
    Name = "ConditionCheck";
    Condition = nullptr;
}

void UConditionCheck::InitializeCondition(ConditionFunction InCondition) {
    Condition = InCondition;
}

EStatus UConditionCheck::Process() {
    if (Condition) {
        bool bConditionMet = Condition();

        UE_LOG(LogTemp, Warning, TEXT("ConditionCheck: %s"), bConditionMet ? TEXT("true") : TEXT("false"));

        if (bConditionMet) {
            return EStatus::SUCCESS;
        } else {
            return EStatus::FAILURE;
        }
    }
    return EStatus::FAILURE;
}

void UConditionCheck::Reset() {
    // No reset needed
}