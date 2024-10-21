#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "Sense.generated.h"

class AEnemyCharacter;

UENUM()
enum class ESenseObject: uint8 {
	PLAYER,
	WEAPON
};


UCLASS()
class AGP_API USense : public UCAction
{
	GENERATED_BODY()

public:

	USense();
	void InitializeType(ESenseObject InType);
	EStatus Process() override;
	void Reset() override;

protected:

	UPROPERTY()
	ESenseObject Type;
	
};
