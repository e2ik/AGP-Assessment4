#pragma once

#include "CoreMinimal.h"
#include "../CAction.h"
#include "GeneratePath.generated.h"

UENUM()
enum class EPathType : uint8
{
	PLAYER,
	WEAPON,
	STATIC,
	FLANK,
	RANDOM
};

UCLASS()
class AGP_API UGeneratePath : public UCAction
{
	GENERATED_BODY()

public:

	UGeneratePath();
	void InitializeType(EPathType InType);
	EStatus Process() override;
	void Reset() override;

private:

	UPROPERTY()
	EPathType Type;	
};
