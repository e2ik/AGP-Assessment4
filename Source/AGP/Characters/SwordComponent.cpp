// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordComponent.h"

#include "BaseMeleeCharacter.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "Evaluation/Blending/MovieSceneBlendType.h"
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

	if (TimeSinceLastSlash >= CooldownDuration)
	{
		bCanSlash = true;
	}

	if (TimeSinceLastSlash < SlashTime)
	{
		SlashImplementation(StartPoint, EndPoint);
	}
}

void USwordComponent::Slash(USceneComponent* Start, USceneComponent* End)
{
	if (bCanSlash) {
		StartPoint = Start;
		EndPoint = End;
		SlashTime = 0;
		ServerSlash(Start, End);
		bCanSlash = false;
		TimeSinceLastSlash = 0;
	}
}

void USwordComponent::ServerSlash_Implementation(USceneComponent* Start, USceneComponent* End)
{
	if (SlashImplementation(Start, End))
	{
		MulticastSlash(Start, End);
	}
}

void USwordComponent::MulticastSlash_Implementation(USceneComponent* Start, USceneComponent* End)
{
	if (!Start || !End) return;
	SlashVisualImplementation(Start->GetComponentLocation(), End->GetComponentLocation());
}

void USwordComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USwordComponent, StartPoint);
	DOREPLIFETIME(USwordComponent, EndPoint);
}

bool USwordComponent::SlashImplementation(USceneComponent* Start, USceneComponent* End)
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());

    // Define the box's size (half extents)
    FVector BoxHalfSize(50.0f, 50.0f, 50.0f);  // Adjust the size as needed

    // Array to store multiple hits
    TArray<FHitResult> HitResults;

    // Calculate rotation and distance between start and end
    FQuat TraceRotation = FRotationMatrix::MakeFromX(End->GetComponentLocation() - Start->GetComponentLocation()).ToQuat();
    FVector TraceStart = Start->GetComponentLocation();
    FVector TraceEnd = End->GetComponentLocation();

    // Perform the box sweep (trace)
    if (GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, TraceRotation, ECC_WorldDynamic, FCollisionShape::MakeBox(BoxHalfSize), QueryParams))
    {
        for (const FHitResult& Hit : HitResults)
        {
            // Check for ABaseMeleeCharacter
            if (ABaseMeleeCharacter* MeleeHitCharacter = Cast<ABaseMeleeCharacter>(Hit.GetActor()))
            {
                if (UHealthComponent* HitCharacterHealth = MeleeHitCharacter->FindComponentByClass<UHealthComponent>())
                {
                    HitCharacterHealth->ApplyDamage(10.0f);
                }
                break;
            }

            // Check for ABaseCharacter
            else if (ABaseCharacter* BaseHitCharacter = Cast<ABaseCharacter>(Hit.GetActor()))
            {
                if (UHealthComponent* HitCharacterHealth = BaseHitCharacter->FindComponentByClass<UHealthComponent>())
                {
                    HitCharacterHealth->ApplyDamage(10.0f);
                }
                break;
            }
        }
    }

    // Call for visual effects of the slash
    SlashVisualImplementation(Start->GetComponentLocation(), End->GetComponentLocation());

    return true;
}




void USwordComponent::SlashVisualImplementation(FVector Start, FVector End)
{
	UE_LOG(LogTemp, Display, TEXT("SLASHINGVISUAL"))
	if (ABaseMeleeCharacter* BaseCharacter = Cast<ABaseMeleeCharacter>(GetOwner()))
	{
		UE_LOG(LogTemp, Display, TEXT("Base character cast successful."))
		BaseCharacter->SwordSlashGraphical();
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Base Character Cast Unsuccessful AHHHHHH"))
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 10.0f);
}

