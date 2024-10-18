#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CSequence.generated.h"


UCLASS()
class AGP_API UCSequence : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCSequence();
	void Initialize(FString InName);
	EStatus Process() override;
};
