#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "GeneratePathAway.generated.h"


UCLASS()
class AGP_API UGeneratePathAway : public UCAction
{
	GENERATED_BODY()
	
public:

	UGeneratePathAway();
	EStatus Process() override;
	void Reset() override;
};
