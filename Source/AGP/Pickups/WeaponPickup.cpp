// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"

#include "../Characters/PlayerCharacter.h"
#include "AGP/Characters/BaseMeleeCharacter.h"
#include "PickupManagerSubsystem.h"
#include "Net/UnrealNetwork.h"

void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		GenerateWeaponPickup();
	}
	UpdateWeaponPickupMaterial();
}

void AWeaponPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	//Super::OnPickupOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, HitInfo);
	// UE_LOG(LogTemp, Display, TEXT("Overlap event occurred on WeaponPickup"))

	if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
	{
		Player->EquipWeapon(true, WeaponStats);
		UPickupManagerSubsystem* PickupManager = GetWorld()->GetSubsystem<UPickupManagerSubsystem>();
		if (PickupManager) { PickupManager->RemovePickup(GetActorLocation()); }
	}
	
	if (ABaseMeleeCharacter* Player = Cast<ABaseMeleeCharacter>(OtherActor))
	{
		Player->EquipSword(true, FSwordStats());
		UPickupManagerSubsystem* PickupManager = GetWorld()->GetSubsystem<UPickupManagerSubsystem>();
		if (PickupManager) { PickupManager->RemovePickup(GetActorLocation()); }
	}
}

void AWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponPickup, WeaponRarity);
	DOREPLIFETIME(AWeaponPickup, WeaponStats);
}

void AWeaponPickup::GenerateWeaponPickup()
{
	WeaponRarity = WeaponRarityPicker();
	TArray<bool> GoodStats;
	switch (WeaponRarity)
	{
	case EWeaponRarity::Legendary:
		GoodStats = WeaponStatPicker(4, 5);
		break;
	case EWeaponRarity::Master:
		GoodStats = WeaponStatPicker(3, 5);
		break;
	case EWeaponRarity::Rare:
		GoodStats = WeaponStatPicker(2, 5);
		break;
	default:
		GoodStats = WeaponStatPicker(0, 5);
		break;
	}

	WeaponStats.Accuracy = GoodStats[0] ? FMath::RandRange(0.98f, 1.0f) : FMath::RandRange(0.9f, 0.98f);
	WeaponStats.FireRate = GoodStats[1] ? FMath::RandRange(0.05f, 0.2f) : FMath::RandRange(0.2f, 1.0f);
	WeaponStats.BaseDamage = GoodStats[2] ? FMath::RandRange(15.0f, 30.0f) : FMath::RandRange(5.0f, 15.0f);
	WeaponStats.MagazineSize = GoodStats[3] ? FMath::RandRange(20, 100) : FMath::RandRange(1, 19);
	WeaponStats.ReloadTime = GoodStats[4] ? FMath::RandRange(0.1f, 1.0f) : FMath::RandRange(1.0f, 4.0f);
}

EWeaponRarity AWeaponPickup::WeaponRarityPicker()
{
	// Rules:
	// 50% chance of Common
	// 30% chance of Rare
	// 15% chance of Master
	// 5% chance of Legendary
	const float RandPercent = FMath::RandRange(0.0f, 1.0f);
	
	if (RandPercent <= 0.5f)
	{
		return EWeaponRarity::Common;
	}
	if (RandPercent <= 0.8f)
	{
		return EWeaponRarity::Rare;
	}
	if (RandPercent <= 0.95f)
	{
		return EWeaponRarity::Master;
	}
	
	return EWeaponRarity::Legendary;
}

TArray<bool> AWeaponPickup::WeaponStatPicker(int32 NumOfGood, int32 NumOfStats)
{
	// Fill the array with the correct number of good and bad stats.
	TArray<bool> GoodStats;
	for (int32 i = 0; i < NumOfStats; i++)
	{
		// Ternary condition: Will add true if I < NumOfGood otherwise add false.
		GoodStats.Add(i < NumOfGood ? true : false);
	}

	// Array shuffling algorithm.
	for (int32 i = 0; i < GoodStats.Num(); i++)
	{
		// Get a random index from the GoodStats array.
		const int32 RandIndex = FMath::RandRange(0, GoodStats.Num() - 1);
		// Then swap the item at that random index with the item in the i index.
		const bool Temp = GoodStats[i];
		GoodStats[i] = GoodStats[RandIndex];
		GoodStats[RandIndex] = Temp;
	}

	return GoodStats;
}

void AWeaponPickup::SetWeaponStats(int32 Level)
{
	switch (Level) {
		case 1:
			WeaponRarity = EWeaponRarity::Common;
			WeaponStats.Accuracy = 0.95f;
			WeaponStats.FireRate = 0.6f;
			WeaponStats.BaseDamage = 15.0f;
			WeaponStats.MagazineSize = 8;
			WeaponStats.ReloadTime = 2.0f;
			break;
		case 2:
			WeaponRarity = EWeaponRarity::Rare;
			WeaponStats.Accuracy = 0.96f;
			WeaponStats.FireRate = 0.4f;
			WeaponStats.BaseDamage = 20.0f;
			WeaponStats.MagazineSize = 15;
			WeaponStats.ReloadTime = 2.0f;
			break;
		case 3:
			WeaponRarity = EWeaponRarity::Master;
			WeaponStats.Accuracy = 0.97f;
			WeaponStats.FireRate = 0.2f;
			WeaponStats.BaseDamage = 30.0f;
			WeaponStats.MagazineSize = 20;
			WeaponStats.ReloadTime = 1.5f;
			break;
		case 4:
			WeaponRarity = EWeaponRarity::Legendary;
			WeaponStats.Accuracy = 0.99f;
			WeaponStats.FireRate = 0.1f;
			WeaponStats.BaseDamage = 40.0f;
			WeaponStats.MagazineSize = 45;
			WeaponStats.ReloadTime = 0.5f;
			break;
		default:
			WeaponRarity = EWeaponRarity::Common;
			WeaponStats.Accuracy = 0.95f;
			WeaponStats.FireRate = 0.6f;
			WeaponStats.BaseDamage = 15.0f;
			WeaponStats.MagazineSize = 8;
			WeaponStats.ReloadTime = 2.0f;
			break;
	}
}