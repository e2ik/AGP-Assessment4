#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBehaviourHeaders.h"
#include "BTComponent.generated.h"

class EnemyCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGP_API UBTComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBTComponent();

	void AttachRoaming();
	void AttachStatic();
	void AttachFlank();
	void UnInitialize();
	void TreeReset();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UCBTree* RootBT;
	UPROPERTY(VisibleAnywhere)
	FString BTName;
	UPROPERTY(VisibleAnywhere)
	EStatus CurrentStatus;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	float TimeAccumulator = 0.0f;
	UPROPERTY(EditAnywhere)
	float TickInterval = 0.5f;

	UPROPERTY()
	bool bIsInitiated = false;
};
