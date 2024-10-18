// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMeleeCharacter.h"
#include "PlayerMeleeCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class AGP_API APlayerMeleeCharacter : public ABaseMeleeCharacter
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* BlockAction;
	
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	float LookSensitivity = 0.5f;

	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<UPlayerCharacterHUD> PlayerHUDClass;
	//
	// UPROPERTY()
	// UPlayerCharacterHUD* PlayerHUD;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ChooseCharacterMesh();
	
	void UpdateHealthBar(float HealthPercent);
	void DrawUI();
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SlashSword(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void Block(const FInputActionValue& Value);
};
