#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CInverter.generated.h"


UCLASS()
class AGP_API UCInverter : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCInverter();
	void Initialize(FString InName);
	EStatus Process() override;
	
};
