#pragma once

#include "CoreMinimal.h"
#include "AGP/BehaviourTree/CLeaf.h"
#include "DecreaseCount.generated.h"

class UAIDirector;

UCLASS()
class AGP_API UDecreaseCount : public UCLeaf
{
	GENERATED_BODY()

public:

	UDecreaseCount();
	EStatus Process() override;
	void Reset() override;
	void PassDirector(UAIDirector* InDirector) { Director = InDirector; }

protected:

	UPROPERTY()
	UAIDirector* Director;
};
