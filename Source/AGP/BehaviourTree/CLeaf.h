#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CAction.h"
#include "CLeaf.generated.h"


UCLASS()
class AGP_API UCLeaf : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UCAction* Action;

	UCLeaf();
	void Initialize(FString InName, UCAction* InAction);
	EStatus Process() override;
	void Reset() override;
	
};
