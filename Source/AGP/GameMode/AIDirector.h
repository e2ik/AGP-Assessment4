#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "AGP/BehaviourTree/AIBehaviourHeaders.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "AIDirector.generated.h"


class ANavigationNode;
class AMultiplayerGameMode;
class UAIAssignSubsystem;
class UAGPGameInstance;
class UPickupManagerSubsystem;
class APlayerCharacter;
class APlayerMeleeCharacter;

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
	void RegisterPlayerDeath(AController* Controller);
	void SetSpawnEnemies(bool Value) { bSpawnEnemies = Value; }
	void IncreaseNumOfEnemies();
	void DecreaseNumOfEnemies();
	void GetPlayerDeathCount(int32& OutDeaths) const { OutDeaths = NumOfPlayerDeaths; }
	void SetPlayerDeathCount(int32 Value) { NumOfPlayerDeaths = Value; }
	void SpawnWeaponAtPlayer();

	UPROPERTY()
	int32 TotalDeathResets = 0;
	UPROPERTY(EditAnywhere)
	bool bTimerCheck;

protected:

	// game specific trackables
	UPROPERTY()
	float GameTime = 0.0f;

	UPROPERTY()
	int32 NumOfPlayerDeaths = 0;
	UPROPERTY(EditAnywhere)
	bool bHasFoundNodes;
	UPROPERTY(EditAnywhere)
	int32 NumOfStarts = 4;
	UPROPERTY(EditAnywhere)
	int32 NumOfEnemies = 4;
	UPROPERTY(EditAnywhere)
	int32 MaxEnemies = 10;
	UPROPERTY(EditAnywhere)
	bool bPlaceStarts;
	UPROPERTY(EditAnywhere)
	bool bSpawnEnemies;
	UPROPERTY(EditAnywhere)
	bool bStartBT;
	UPROPERTY(EditAnywhere)
	bool bBTConstructed;

	UPROPERTY(EditAnywhere)
	float DesiredTickInterval = 5.0f;
	UPROPERTY()
	float LastTickTime = 0.0f;

	UPROPERTY()
	TArray<ANavigationNode*> Nodes;

	UPROPERTY()
	TArray<FVector> SpawnRadiusLocations;

	UPROPERTY()
	TMap<int32, FString> PlayerIndexMap;
	UPROPERTY()
	TMap<int32, int32> PlayerControllerMap;

	UPROPERTY(VisibleAnywhere)
	UCBTree* RootBT;
	UPROPERTY(VisibleAnywhere)
	FString BTName;
	UPROPERTY(VisibleAnywhere)
	EStatus CurrentStatus;

private:

	void RunCustomTick();

	void GetRandomNode();
	void FindAllNodes();
	void PlacePlayerStarts(const FVector& Location);
	void GenerateEnemySpawn(ANavigationNode* CenterNode);
	void SpawnEnemies();
	void SetPlayersMap();
	void RunBT();
	void ConstructBT();

	float LastExecutionTime = 0.0f;
	
};
