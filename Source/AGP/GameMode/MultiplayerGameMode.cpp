// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "AGP/Characters/PlayerCharacter.h"
#include "AGP/Characters/EnemyCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

void AMultiplayerGameMode::RespawnPlayer(AController* Controller)
{
	if (Controller)
	{
		if (APlayerCharacter* CurrentlyPossessedCharacter = Cast<APlayerCharacter>(Controller->GetPawn()))
		{
			Controller->UnPossess();
			CurrentlyPossessedCharacter->Destroy();
			RestartPlayer(Controller);
			if (APlayerCharacter* NewPossessedCharacter = Cast<APlayerCharacter>(Controller->GetPawn()))
			{
				NewPossessedCharacter->ChooseCharacterMesh();
				NewPossessedCharacter->DrawUI();
			}
		}
	}
	
}

void AMultiplayerGameMode::RespawnEnemy(AController* Controller)
{
	if (Controller)
	{
		if (AEnemyCharacter* CurrentlyPossessedCharacter = Cast<AEnemyCharacter>(Controller->GetPawn()))
		{
			Controller->UnPossess();
			CurrentlyPossessedCharacter->Destroy();
		}
	}

	// Grab a spawn location
	AActor* SpawnLocationActor = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		SpawnLocationActor = *It;
		break;
	}

	if (SpawnLocationActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = nullptr;
		SpawnParams.bNoFail = true;

		AEnemyCharacter* NewEnemyCharacter = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyCharacterClass, SpawnLocationActor->GetActorLocation(), SpawnLocationActor->GetActorRotation(), SpawnParams);
		if (NewEnemyCharacter)
		{
			Controller->Possess(NewEnemyCharacter);
		}
	}
}
