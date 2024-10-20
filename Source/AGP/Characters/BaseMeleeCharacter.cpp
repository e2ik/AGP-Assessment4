// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMeleeCharacter.h"
#include "HealthComponent.h"
#include "AGP/GameMode/MultiplayerGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseMeleeCharacter::ABaseMeleeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
}

// Called when the game starts or when spawned
void ABaseMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABaseMeleeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseMeleeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ABaseMeleeCharacter::HasSword()
{
	return SwordComponent != nullptr;
}


void ABaseMeleeCharacter::Slash()
{
	if (!HasSword()) return;
	SwordComponent->Slash(SwordStartPosition, SwordEndPosition);
}

void ABaseMeleeCharacter::OnDeath()
{
	if (GetLocalRole() != ROLE_Authority) return;

	//TODO: Make this reference the new player character type or adjust player character.
	// if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this))
	// {
	// 	if (AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode()))
	// 	{
	// 		GameMode->RespawnPlayer(GetController());
	// 	}
	// }

	//TODO: Make this reference the new enemy character or adjust enemy character.
	// if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(this))
	// {
	// 	if (AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode()))
	// 	{
	// 		GameMode->RespawnEnemy(GetController());
	// 	}
	// }
}

void ABaseMeleeCharacter::EquipSword(bool bEquipWeapon, const FSwordStats& SwordStats)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EquipSwordImplementation(bEquipWeapon, SwordStats);
		MulticastEquipWeapon(bEquipWeapon, SwordStats);
	}
}

void ABaseMeleeCharacter::EquipSwordImplementation(bool bEquipWeapon, const FSwordStats& SwordStats)
{
	if (bEquipWeapon && !HasSword())
	{
		SwordComponent = NewObject<USwordComponent>(this);
		SwordComponent->RegisterComponent();
	}
	else if (!bEquipWeapon && HasSword())
	{
		SwordComponent->UnregisterComponent();
		SwordComponent = nullptr;
	}
}

void ABaseMeleeCharacter::MulticastEquipWeapon_Implementation(bool bEquipWeapon, const FSwordStats& SwordStats)
{
	EquipSwordGraphical(bEquipWeapon, SwordStats);
}

void ABaseMeleeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseMeleeCharacter, SwordComponent);
}