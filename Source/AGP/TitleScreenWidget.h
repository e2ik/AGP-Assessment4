// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/PlayerMeleeCharacter.h"
#include "TitleScreenWidget.generated.h"


class UAGPGameInstance;

/**
 * 
 */
UCLASS()
class AGP_API UTitleScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void OnHostButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnJoinButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnMeleeButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnGunButtonPressed();

	void RespawnPlayer(APlayerController* PlayerController);
	
	UFUNCTION(Server, Reliable)
	void ServerRespawnPlayer(APlayerController* PlayerController);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRespawnPlayer(APlayerController* PlayerController);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlayerMeleeCharacter> PlayerMeleeCharacterClass;
};
