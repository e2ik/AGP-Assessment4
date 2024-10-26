#pragma once

#include "CoreMinimal.h"
#include "AGP/BehaviourTree/CLeaf.h"
#include "IncreaseCount.generated.h"

class UAIDirector;

UCLASS()
class AGP_API UIncreaseCount : public UCLeaf
{
	GENERATED_BODY()

public:

	UIncreaseCount();
	EStatus Process() override;
	void Reset() override;
	void PassDirector(UAIDirector* InDirector) { Director = InDirector; }

protected:

	UPROPERTY()
	UAIDirector* Director;
};
