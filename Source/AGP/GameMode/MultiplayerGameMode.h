// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

class APlayerCharacter;
class APlayerMeleeCharacter;
class AEnemyCharacter;
class APlayerStart;
class APlayerController;
class UAIAssignSubsystem;
class UAGPGameInstance;


UCLASS()
class AGP_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	void RespawnPlayer(AController* Controller);
	void RespawnEnemy(AController* Controller);
	void SpawnPlayers();
	void SpawnEnemy(const FVector& Location);
	FVector GetPlayerStartLocation();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlayerCharacter> PlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlayerMeleeCharacter> PlayerMeleeCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyCharacter> EnemyCharacterClass;
	
};
