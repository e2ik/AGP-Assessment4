#pragma once

#include "CoreMinimal.h"
#include "AGP/BehaviourTree/CLeaf.h"
#include "Spawn.generated.h"

class UAIDirector;

UCLASS()
class AGP_API USpawn : public UCLeaf
{
	GENERATED_BODY()

public:

	USpawn();
	EStatus Process() override;
	void Reset() override;
	void PassDirector(UAIDirector* InDirector) { Director = InDirector; }

protected:

	UPROPERTY()
	UAIDirector* Director;
};
