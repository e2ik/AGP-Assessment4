#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CBTree.generated.h"


UCLASS()
class AGP_API UCBTree : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCBTree();
	void Initialize(FString InName);
	EStatus Process() override;
	
};
