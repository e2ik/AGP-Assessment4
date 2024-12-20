#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "ReloadWeapon.generated.h"

class AEnemyCharacter;

UCLASS()
class AGP_API UReloadWeapon : public UCAction
{
	GENERATED_BODY()

public:

	UReloadWeapon();
	EStatus Process() override;
	void Reset() override;
	
};
