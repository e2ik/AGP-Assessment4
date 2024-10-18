#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "ConditionCheck.generated.h"


UCLASS()
class AGP_API UConditionCheck : public UCAction
{
	GENERATED_BODY()

public:

	UConditionCheck();

	typedef TFunction<bool()> ConditionFunction;

	void InitializeCondition(ConditionFunction InCondition);
	EStatus Process() override;
	void Reset() override;

protected:
	ConditionFunction Condition;
	
};
