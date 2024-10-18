// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"
#include "AGPGameInstance.generated.h"

class AWeaponPickup;
class UBTComponent;
class AEnemyCharacter;
/**
 * 
 */
UCLASS()
class AGP_API UAGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UClass* GetWeaponPickupClass() const;
	UClass* GetBTClass() const;
	UClass* GetEnemyClass() const;
	void SpawnGroundHitParticle(const FVector& SpawnLocation);
	void SpawnCharacterHitParticle(const FVector& SpawnLocation);
	void PlayGunshotSoundAtLocation(const FVector& Location);
	void PlayGunshotSound2D();

protected:

	UPROPERTY(EditDefaultsOnly, Category="Pickup Classes")
	TSubclassOf<AWeaponPickup> WeaponPickupClass;

	UPROPERTY(EditDefaultsOnly, Category="Behaviour Tree")
	TSubclassOf<UBTComponent> BTClass;

	UPROPERTY(EditDefaultsOnly, Category="Enemy Classes")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* GroundHitParticle;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* CharacterHitParticle;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* GunshotSoundCue;
	
};
