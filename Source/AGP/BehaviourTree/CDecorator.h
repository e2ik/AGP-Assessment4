#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "GameFramework/Actor.h"
#include "CDecorator.generated.h"


UCLASS()
class AGP_API UCDecorator : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	typedef TFunction<bool()> ConditionFunction;
	void InitializeCondition(ConditionFunction InCondition);

	UCDecorator();
	void Initialize(FString InName);
	EStatus Process() override;
	void SetOwner(AActor* InOwner);


protected:
	UPROPERTY()
	AActor* Owner;
	
	ConditionFunction Condition;
	
};
