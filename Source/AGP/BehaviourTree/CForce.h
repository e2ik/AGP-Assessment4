#pragma once

#include "CoreMinimal.h"
#include "CNode.h"
#include "CForce.generated.h"

UENUM()
enum class EForceType: uint8 {
	SUCCESS,
	FAILURE
};

UCLASS()
class AGP_API UCForce : public UCNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EStatus CurrentStatus;

	UCForce();
	void Initialize(FString InName);
	void InitializeType(EForceType InType);
	EStatus Process() override;

protected:
	UPROPERTY()
	EForceType Type;	
};
