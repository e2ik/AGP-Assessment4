#pragma once

#include "CoreMinimal.h"
#include "AGP/BehaviourTree/CLeaf.h"
#include "SpawnWeapon.generated.h"

class UAIDirector;

UCLASS()
class AGP_API USpawnWeapon : public UCLeaf
{
	GENERATED_BODY()

public:

	USpawnWeapon();
	EStatus Process() override;
	void Reset() override;
	void PassDirector(UAIDirector* InDirector) { Director = InDirector; }

protected:

	UPROPERTY()
	UAIDirector* Director;
};
