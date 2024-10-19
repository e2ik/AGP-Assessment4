#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "AGP/BehaviourTree/AIBehaviourHeaders.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "AIDirector.generated.h"


class ANavigationNode;
class AMultiplayerGameMode;

UCLASS()
class AGP_API UAIDirector : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	UAIDirector();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

protected:

	UPROPERTY(EditAnywhere)
	bool bHasFoundNodes;
	UPROPERTY(EditAnywhere)
	int32 NumOfStarts = 4;
	UPROPERTY(EditAnywhere)
	bool bPlaceStarts;

	UPROPERTY(EditAnywhere)
	float DesiredTickInterval = 5.0f;
	UPROPERTY()
	float LastTickTime = 0.0f;

	UPROPERTY()
	TArray<ANavigationNode*> Nodes;

private:

	void RunCustomTick();

	void GetRandomNode();
	void FindAllNodes();
	void PlacePlayerStarts(const FVector& Location);
	
};
