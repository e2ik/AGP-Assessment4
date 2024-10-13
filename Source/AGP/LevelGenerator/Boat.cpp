// Fill out your copyright notice in the Description page of Project Settings.


#include "Boat.h"

#include "BoatNavigation.h"
#include "VectorTypes.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

// Sets default values
ABoat::ABoat()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABoat::BeginPlay()
{
	Super::BeginPlay();
	BoatNavigation = GetWorld()->GetSubsystem<UBoatNavigation>();
}

// Called every frame
void ABoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveAlongPath();
}

// Called to bind functionality to input
void ABoat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoat::MoveAlongPath()
{
	if (!TargetBoatNode || !LastBoatNode || !CurrentBoatNode)
	{
		SetCurrentBoatTarget(BoatNavigation->GetNewBoatPath());
		return;
	}

	float DistanceToCurrentTarget = FVector::Distance(CurrentBoatNode->GetActorLocation(), LastBoatNode->GetActorLocation());
	float DistanceToNextTarget = FVector::Distance(LastBoatNode->GetActorLocation(), TargetBoatNode->GetActorLocation());
	if (1.75 * DistanceToCurrentTarget <= DistanceToNextTarget) {
		FVector MovementDirection = CurrentBoatNode->GetActorLocation() - GetActorLocation();
		MovementDirection.Normalize();
		AddMovementInput(MovementDirection * 1/10 * BoatSpeed);
	} else
	{
		FVector StartMovementDirection = LastBoatNode->GetActorLocation() - CurrentBoatNode->GetActorLocation();
		FVector EndMovementDirection = TargetBoatNode->GetActorLocation() - CurrentBoatNode->GetActorLocation();
		StartMovementDirection.Normalize();
		EndMovementDirection.Normalize();
		float DistanceOfCurve = FVector::Distance(LastBoatNode->GetActorLocation(), TargetBoatNode->GetActorLocation());
		float DistanceToCurrentTargetNode = FVector::Distance(TargetBoatNode->GetActorLocation(), GetActorLocation());
		float Alpha = DistanceToCurrentTargetNode/DistanceOfCurve;
		UE_LOG(LogTemp, Display, TEXT("CurrentAlpha: %f"), Alpha);
		FVector MovementDirection = FMath::Lerp(StartMovementDirection, EndMovementDirection, Alpha);
		AddMovementInput(MovementDirection * 1/10 * BoatSpeed);
	}
	
	if (FVector::Distance(GetActorLocation(), CurrentBoatNode->GetActorLocation()) < PathfindingError)
	{
		GetNextBoatTarget();
	}
}

void ABoat::SetCurrentBoatTarget(ABoatNode* NewTarget)
{
	SetActorLocation(NewTarget->GetActorLocation());
	LastBoatNode = NewTarget;
	TArray<ABoatNode*> PossibleNodes = NewTarget->GetConnectedNodes();
	if (PossibleNodes.Num() == 1)
	{
		CurrentBoatNode = PossibleNodes[0];
	} else if (PossibleNodes.Num() > 1)
	{
		CurrentBoatNode = PossibleNodes[FMath::RandRange(0, PossibleNodes.Num() - 1)];
	}
	PossibleNodes = CurrentBoatNode->GetConnectedNodes();
	if (PossibleNodes.Num() == 1)
	{
		TargetBoatNode = PossibleNodes[0];
	} else if (PossibleNodes.Num() > 1)
	{
		TargetBoatNode = PossibleNodes[FMath::RandRange(0, PossibleNodes.Num() - 1)];
	}
}

void ABoat::GetNextBoatTarget()
{
	TArray<ABoatNode*> PossibleNodes = TargetBoatNode->GetConnectedNodes();
	PossibleNodes.Remove(LastBoatNode);
	PossibleNodes.Remove(CurrentBoatNode);
	PossibleNodes.Remove(TargetBoatNode);
	if (PossibleNodes.IsEmpty())
	{
		Swap(TargetBoatNode, LastBoatNode);
	}
	else if (PossibleNodes.Num() == 1)
	{
		LastBoatNode = CurrentBoatNode;
		CurrentBoatNode = TargetBoatNode;
		TargetBoatNode = PossibleNodes[0];
	}
	else
	{
		LastBoatNode = CurrentBoatNode;
		CurrentBoatNode = TargetBoatNode;
		TargetBoatNode = PossibleNodes[FMath::RandRange(0, PossibleNodes.Num() - 1)];
	}
}
