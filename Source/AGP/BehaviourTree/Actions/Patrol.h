#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "Patrol.generated.h"

class AEnemyCharacter;

UCLASS()
class AGP_API UPatrol : public UCAction
{
	GENERATED_BODY()

public:

	UPatrol();
	EStatus Process() override;
	void Reset() override;
	
};
