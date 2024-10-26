// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleScreenWidget.h"
#include "GameMode/AGPGameInstance.h"
#include "GameMode/MultiplayerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "AGP/GameMode/AGPGameInstance.h"

void UTitleScreenWidget::OnHostButtonPressed()
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetGameInstance()))
	{
		GameInstance->CreateSession("GameSession");
	}
}

void UTitleScreenWidget::OnJoinButtonPressed()
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetGameInstance()))
	{
		GameInstance->ClientTravelToSession(GetOwningLocalPlayer()->GetControllerId(), "GameSession");
	}
}

void UTitleScreenWidget::OnMeleeButtonPressed()
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetGameInstance()))
	{
		GameInstance->SetSelectedPawnClass(GameInstance->GetPlayerMeleeClass());
	}
	GetWorld()->ServerTravel("/Game/Levels/ProcGenLevel?listen");
}

void UTitleScreenWidget::OnGunButtonPressed()
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetGameInstance()))
	{
		GameInstance->SetSelectedPawnClass(GameInstance->GetPlayerClass());
	}
	GetWorld()->ServerTravel("/Game/Levels/ProcGenLevel?listen");
}


void UTitleScreenWidget::ServerRespawnPlayer_Implementation(APlayerController* PlayerController)
{
	if(AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RespawnPlayer(PlayerController);
	}
}

void UTitleScreenWidget::MulticastRespawnPlayer_Implementation(APlayerController* PlayerController)
{
	ServerRespawnPlayer(PlayerController);
}

void UTitleScreenWidget::RespawnPlayer(APlayerController* PlayerController)
{
	PlayerController->UnPossess();

	APlayerMeleeCharacter* NewMeleeCharacter = GetWorld()->SpawnActor<APlayerMeleeCharacter>(PlayerMeleeCharacterClass, FVector(0.0), FRotator(0.0));
	PlayerController->Possess(NewMeleeCharacter);

	// if (NewMeleeCharacter)
	// {
	// 	NewMeleeCharacter->ChooseCharacterMesh();
	// 	NewMeleeCharacter->DrawUI();
	// }
}
