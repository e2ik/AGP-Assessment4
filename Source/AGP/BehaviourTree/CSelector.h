#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CSelector.generated.h"


UCLASS()
class AGP_API UCSelector : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCSelector();
	void Initialize(FString InName);
	EStatus Process() override;
	
};
