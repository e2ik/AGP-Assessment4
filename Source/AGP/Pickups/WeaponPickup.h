// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "AGP/Characters/WeaponComponent.h"
#include "WeaponPickup.generated.h"

UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common,
	Rare,
	Master,
	Legendary
};

/**
 * 
 */
UCLASS()
class AGP_API AWeaponPickup : public APickupBase
{
	GENERATED_BODY()

public:

	void SetWeaponStats(int32 Level);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponPickupMaterial();

protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	EWeaponRarity WeaponRarity = EWeaponRarity::Common;
	UPROPERTY(Replicated)
	FWeaponStats WeaponStats;

	virtual void BeginPlay() override;
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:

	void GenerateWeaponPickup();
	EWeaponRarity WeaponRarityPicker();
	TArray<bool> WeaponStatPicker(int32 NumOfGood, int32 NumOfStats);
	
};
