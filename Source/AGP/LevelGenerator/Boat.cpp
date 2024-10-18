// Fill out your copyright notice in the Description page of Project Settings.


#include "Boat.h"

#include "BoatNavigation.h"
#include "VectorTypes.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"
#include "Net/UnrealNetwork.h"

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
	if (!HasAuthority()) return;
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

	FVector Direction;
	FVector StartMovementDirection = (CurrentBoatNode->GetActorLocation() - LastBoatNode->GetActorLocation()).GetSafeNormal();
	FVector EndMovementDirection = (TargetBoatNode->GetActorLocation() - CurrentBoatNode->GetActorLocation()).GetSafeNormal();
	float DotProduct = FVector::DotProduct(StartMovementDirection, EndMovementDirection);
	bool bIsStraight = FMath::Abs(DotProduct - 1.0f) < KINDA_SMALL_NUMBER;
	if (bIsStraight)
	{
		Direction = (CurrentBoatNode->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("SDirection: %f, %f, %f"), Direction.X, Direction.Y, Direction.Z);
		if (FVector::Distance(GetActorLocation(), CurrentBoatNode->GetActorLocation()) < PathfindingError)
		{
			GetNextBoatTarget();
		}
	}
	else
	{
		FVector Direction1 = (CurrentBoatNode->GetActorLocation() - LastBoatNode->GetActorLocation()).GetSafeNormal();
		FVector Direction2 = (TargetBoatNode->GetActorLocation() - CurrentBoatNode->GetActorLocation()).GetSafeNormal();
		float DistanceToTarget = FVector::Distance(GetActorLocation(), TargetBoatNode->GetActorLocation());
		float TotalDistance = FVector::Distance(LastBoatNode->GetActorLocation(), TargetBoatNode->GetActorLocation());
		float Radius = TotalDistance / FMath::Sqrt(2.0f);
		float CosTheta = (2 * FMath::Square(Radius) - FMath::Square(DistanceToTarget)) / (2 * FMath::Square(Radius));
		float AngleRadians = FMath::Acos(CosTheta);
		float ArcLength = Radius * AngleRadians;
		float TotalAngleRadians = FMath::DegreesToRadians(90.0f);
		float TotalArcLength = TotalAngleRadians * Radius;
		float Alpha = 1 - (ArcLength/TotalArcLength);
		Alpha = Alpha > 0 ? Alpha : 0;
		Direction = FMath::Lerp(Direction1, Direction2, Alpha).GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("CDirection: %f, %f, %f"), Direction.X, Direction.Y, Direction.Z);
		if (FVector::Distance(GetActorLocation(), TargetBoatNode->GetActorLocation()) < PathfindingError * 1.9)
		{
			GetNextBoatTarget();
		}
	}
	AddMovementInput(Direction * 1/10 * BoatSpeed);
}

void ABoat::SetCurrentBoatTarget(ABoatNode* NewTarget)
{
	SetActorLocation(NewTarget->GetActorLocation());
	LastBoatNode = NewTarget;
	TArray<ABoatNode*> PossibleNodes = NewTarget->GetConnectedNodes();
	PossibleNodes.Remove(LastBoatNode);
	if (PossibleNodes.Num() == 1)
	{
		CurrentBoatNode = PossibleNodes[0];
	} else if (PossibleNodes.Num() > 1)
	{
		CurrentBoatNode = PossibleNodes[FMath::RandRange(0, PossibleNodes.Num() - 1)];
	}
	PossibleNodes = CurrentBoatNode->GetConnectedNodes();
	PossibleNodes.Remove(LastBoatNode);
	PossibleNodes.Remove(CurrentBoatNode);
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