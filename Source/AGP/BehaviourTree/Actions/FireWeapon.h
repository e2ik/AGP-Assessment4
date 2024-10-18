#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "FireWeapon.generated.h"


UCLASS()
class AGP_API UFireWeapon : public UCAction
{
	GENERATED_BODY()

public:

	UFireWeapon();
	EStatus Process() override;
	void Reset() override;
	
};
