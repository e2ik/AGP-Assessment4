// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SwordComponent.h"
#include "GameFramework/Character.h"
#include "AGP/GameMode/MultiplayerGameMode.h"
#include "BaseMeleeCharacter.generated.h"

class USwordComponent;
class UHealthComponent;

UCLASS()
class AGP_API ABaseMeleeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMeleeCharacter();

	UFUNCTION(BlueprintCallable)
	bool HasSword();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayRedFlash();

	UFUNCTION(NetMultiCast, Reliable)
	void MulticastDisplayRedFlash();

	void EquipSword(bool bEquipWeapon, const FSwordStats& SwordStats = FSwordStats());

	UFUNCTION(BlueprintImplementableEvent)
	void EquipSwordGraphical(bool bEquipWeapon, FSwordStats SwordStats);

	UFUNCTION(BlueprintImplementableEvent)
	void SwordSlashGraphical();

	UFUNCTION(BlueprintImplementableEvent)
	void BlockGraphical();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnDeath();

	void Block();
	void StopBlock();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SwordStartPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SwordEndPosition;

	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(Replicated)
	USwordComponent* SwordComponent;

	void Slash();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void EquipSwordImplementation(bool bEquipWeapon, const FSwordStats& SwordStats = FSwordStats());

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(bool bEquipWeapon, const FSwordStats& SwordStats = FSwordStats());
	
	friend class AMultiplayerGameMode;
};