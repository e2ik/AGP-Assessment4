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
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); }

	bool GetHasFoundNodes() const { return bHasFoundNodes; }
	void SetHasFoundNodes(bool Value) { bHasFoundNodes = Value; }
	bool GetPlaceStarts() const { return bPlaceStarts; }
	void SetPlaceStarts(bool Value) { bPlaceStarts = Value; }

protected:

	// game specific trackables
	UPROPERTY()
	float GameTime = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bHasFoundNodes;
	UPROPERTY(EditAnywhere)
	int32 NumOfStarts = 4;
	UPROPERTY(EditAnywhere)
	int32 NumOfEnemies = 4;
	UPROPERTY(EditAnywhere)
	bool bPlaceStarts;
	UPROPERTY(EditAnywhere)
	bool bSpawnEnemies;

	UPROPERTY(EditAnywhere)
	float DesiredTickInterval = 5.0f;
	UPROPERTY()
	float LastTickTime = 0.0f;

	UPROPERTY()
	TArray<ANavigationNode*> Nodes;

	UPROPERTY()
	TArray<FVector> SpawnRadiusLocations;

private:

	void RunCustomTick();

	void GetRandomNode();
	void FindAllNodes();
	void PlacePlayerStarts(const FVector& Location);
	void GenerateEnemySpawn(ANavigationNode* CenterNode);
	void SpawnEnemies();
	
};
