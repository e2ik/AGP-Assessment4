#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIAssignSubsystem.generated.h"

class AEnemyCharacter;
class APlayerCharacter;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleAnywhere)
    FVector Location;

    UPROPERTY(VisibleAnywhere)
    FRotator Rotation;


    FEnemySpawnInfo() {}

    FEnemySpawnInfo(const FVector& InLocation, const FRotator& InRotation)
        : Location(InLocation), Rotation(InRotation) {}

};

UCLASS()
class AGP_API UAIAssignSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& inWorld) override;
	void NotifyPlayerSensed(bool Sensed, AEnemyCharacter* Enemy);
	void NotifyPlayerIsClose(bool Close, AEnemyCharacter* Enemy);
	void NotifyDeath(AEnemyCharacter* Enemy);

protected:

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemyCharacter*> Enemies;

    UPROPERTY(Transient, VisibleAnywhere)
    TArray<FEnemySpawnInfo> EnemySpawnInfos;

	UPROPERTY()
	bool bCanSeePlayer = false;

private:
	friend class AMultiplayerGameMode;
	void AssignAI();
	void GetEnemies();
	void CheckBTComponent(AEnemyCharacter* Enemy);
	void WaitBeforeSwap(AEnemyCharacter* Enemy);
	UPROPERTY()
	bool bPlayerIsClose = false;

	void RespawnEnemies();
	
};
