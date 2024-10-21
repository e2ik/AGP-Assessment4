#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIAssignSubsystem.generated.h"

class AEnemyCharacter;
class APlayerCharacter;
class UBTComponent;
class UAGPGameInstance;

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
	
};
