#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CNode.generated.h"


UENUM(BlueprintType)
enum class EStatus : uint8 {
	IDLE,
	RUNNING,
	FAILURE,
	SUCCESS
};

UCLASS()
class AGP_API UCNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Name;
	UPROPERTY()
	EStatus Status;
	UPROPERTY()
	TArray<UCNode*> Children;

	UCNode();
	void Initialize(FString InName);
	EStatus GetNodeStatus() const;
	void SetNodeStatus(EStatus NewStatus);
	UCNode* GetChild(int32 Index);

	void AddChild(UCNode* Child);
	virtual EStatus Process();
	virtual void Reset();

protected:

	int32 CurrentChild;


private:
	
	
};
