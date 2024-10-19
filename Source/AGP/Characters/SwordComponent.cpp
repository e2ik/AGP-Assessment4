// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordComponent.h"

#include "BaseMeleeCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USwordComponent::USwordComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USwordComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USwordComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimeSinceLastSlash += DeltaTime;
	if (StartPoint && EndPoint)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint->GetComponentLocation(), EndPoint->GetComponentLocation(), ECC_WorldStatic, QueryParams);
		if(HitResult.GetActor())
		{
			UE_LOG(LogTemp, Display, TEXT("Hit something"))
		}
		if (TimeSinceLastSlash > SlashTime)
		{
			StartPoint = nullptr;
			EndPoint = nullptr;
		}
	}
	// ...
}

void USwordComponent::Slash(USceneComponent* Start, USceneComponent* End)
{
	StartPoint = Start;
	EndPoint = End;
	SlashTime = 0;
	SlashImplementation(Start, End);
	UE_LOG(LogTemp, Display, TEXT("SLASHING"))
}

void USwordComponent::ServerSlash_Implementation(USceneComponent* Start, USceneComponent* End)
{
	
}

void USwordComponent::MulticastSlash_Implementation(USceneComponent* Start, USceneComponent* End)
{
	
}

void USwordComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USwordComponent, StartPoint);
	DOREPLIFETIME(USwordComponent, EndPoint);
}

void USwordComponent::SlashImplementation(USceneComponent* Start, USceneComponent* End)
{
	SlashVisualImplementation(Start, End);
}

void USwordComponent::SlashVisualImplementation(USceneComponent* Start, USceneComponent* End)
{
	UE_LOG(LogTemp, Display, TEXT("SLASHINGVISUAL"))
	if (ABaseMeleeCharacter* BaseCharacter = Cast<ABaseMeleeCharacter>(GetOwner()))
	{
		UE_LOG(LogTemp, Display, TEXT("Base character cast successful."))
		BaseCharacter->SwordSlashGraphical();
	}
}

