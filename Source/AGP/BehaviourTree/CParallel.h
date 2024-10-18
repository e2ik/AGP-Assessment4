#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CParallel.generated.h"


UCLASS()
class AGP_API UCParallel : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCParallel();
	void Initialize(FString InName);
	EStatus Process() override;
	
};
