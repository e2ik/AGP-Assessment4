#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "CNode.h"
#include "CAction.generated.h"


UCLASS()
class AGP_API UCAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Name;

	UCAction();
	virtual void Initialize(FString InName);
	virtual EStatus Process();
	virtual void Reset();
	void SetOwner(AActor* InOwner);

protected:

	UPROPERTY()
	AActor* Owner;
	
};
