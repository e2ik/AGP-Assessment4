#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "FollowPath.generated.h"


UCLASS()
class AGP_API UFollowPath : public UCAction
{
	GENERATED_BODY()

public:

	UFollowPath();
	EStatus Process() override;
	void Reset() override;
};
