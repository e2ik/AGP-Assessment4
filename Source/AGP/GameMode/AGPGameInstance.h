// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NiagaraSystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Sound/SoundCue.h"
#include "AGPGameInstance.generated.h"

class AWeaponPickup;
class UBTComponent;
class AEnemyCharacter;
class APlayerCharacter;
class APlayerMeleeCharacter;
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
	UClass* GetPlayerClass() const;
	UClass* GetPlayerMeleeClass() const;
	void SpawnGroundHitParticle(const FVector& SpawnLocation);
	void SpawnCharacterHitParticle(const FVector& SpawnLocation);
	void SpawnBlockEffect(const FVector& SpawnLocation);
	void SpawnGunEffect(const FVector& StartLocation, const FVector& EndLocation, int32 Color);
	void PlayGunshotSoundAtLocation(const FVector& Location);
	void PlayGunshotSound2D();
	virtual void Init() override;
	void CreateSession(FName SessionName);
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void ServerTravel();
	void ClientTravel();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Setup")
    TSubclassOf<APawn> SelectedPawnClass;

	void SetSelectedPawnClass(TSubclassOf<APawn> PawnClass);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Pickup Classes")
	TSubclassOf<AWeaponPickup> WeaponPickupClass;

	UPROPERTY(EditDefaultsOnly, Category="Behaviour Tree")
	TSubclassOf<UBTComponent> BTClass;

	UPROPERTY(EditDefaultsOnly, Category="Enemy Classes")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category="Player Classes")
	TSubclassOf<APlayerCharacter> PlayerClass;

	UPROPERTY(EditDefaultsOnly, Category="Player Classes")
	TSubclassOf<APlayerMeleeCharacter> PlayerMeleeClass;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* GroundHitParticle;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* CharacterHitParticle;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* BlockEffect;

	UPROPERTY(EditDefaultsOnly, Category="Particle Systems")
	UNiagaraSystem* GunEffect;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* GunshotSoundCue;

private:
	IOnlineSubsystem* OnlineSubsystem;

	IOnlineSessionPtr Session;
	
};
