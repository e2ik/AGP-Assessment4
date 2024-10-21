#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "ClearPath.generated.h"

class AEnemyCharacter;

UCLASS()
class AGP_API UClearPath : public UCAction
{
	GENERATED_BODY()

public:

	UClearPath();
	EStatus Process() override;
	void Reset() override;
	
};
