// Fill out your copyright notice in the Description page of Project Settings.


#include "WallGenerator.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"


void AWallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldUpdate)
	{
		TArray<int> Map = {
			1, 1, 1, 1, 1, 1, 1, 1, 0, 0, //WallA -> Across this row
			1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			0, 1, 1, 1, 1, 1, 1, 1, 1, 0, //WallD is this side
			1, 1, 1, 1, 1, 1, 1, 1, 1, 0, //WallB is the other side
			1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 0, 0, 1, //WallC -> Across this row
		};
		DeleteMeshes();
		GenerateWalls(DefaultWidth, DefaultHeight, DefaultScale, Map);
		bShouldUpdate = false;
	}
}

void AWallGenerator::GenerateWalls(int Width, int Height, float Scale, TArray<int> Map)
{
	DeleteMeshes();
	TArray<int> WallA;
	TArray<int> WallB;
	TArray<int> WallC;
	TArray<int> WallD;

	for (int x = 0; x < Width; x++)
	{
		WallA.Add(Map[x]);
	}
	for (int x = Height - 1; x >= 0; x--)
    {
    	WallB.Add(Map[x * Width]);
    }
	for (int x = 0; x < Width; x++)
	{
		WallC.Add(Map[(Map.Num() - 1) - x]);
	}
	for (int x = Height - 1; x >= 0; x--)
	{
		WallD.Add(Map[Map.Num() - 1 - (x * Width)]);
	}
	
	int RandWallSelection = FMath::RandRange(0, Walls.Num() - 1);
	float CityWidth = Width * Scale;
	float CityHeight = Height * Scale;
	
	FVector LocationA = FVector(-CityWidth/2, -CityHeight/2, 0.0) + GetActorLocation();
	FVector LocationB = FVector(CityWidth/2, -CityHeight/2, 0.0) + GetActorLocation();
	FVector LocationC = FVector(CityWidth/2, CityHeight/2, 0.0) + GetActorLocation();
	FVector LocationD = FVector(-CityWidth/2, CityHeight/2, 0.0) + GetActorLocation();

	SpawnFullWall(WallA, Scale, LocationA, LocationB, RandWallSelection);
	SpawnFullWall(WallB, Scale, LocationD, LocationA, RandWallSelection);
	SpawnFullWall(WallC, Scale, LocationC, LocationD, RandWallSelection);
	SpawnFullWall(WallD, Scale, LocationB, LocationC, RandWallSelection);
}

void AWallGenerator::SpawnFullWall(TArray<int> Key, float Scale, FVector StartPosition, FVector EndPosition, int RandWallSelection)
{
	FVector CurrentStartLocation = StartPosition;
	FVector DirectionNormal = (EndPosition - StartPosition).GetSafeNormal();
	FVector CurrentEndLocation = StartPosition;
	FVector WallDimensions = GetMeshDimensions(Walls[RandWallSelection]);
	FVector TowerOffset = FVector(WallDimensions.Y / 2 * DirectionNormal.Y, WallDimensions.Y / 2 * -DirectionNormal.X, 0.0);
	FVector WallOffset = GetMeshDimensions(Tower) / 3;
	WallOffset *= DirectionNormal;
	FVector InverseTowerOffset = FVector(WallDimensions.Y / 2 * DirectionNormal.X, WallDimensions.Y / 2 * DirectionNormal.Y, 0.0);
	CurrentStartLocation -= InverseTowerOffset;
	UE_LOG(LogTemp, Warning, TEXT("Direction Normal: X: %f, Y: %f, Z: %f"), TowerOffset.X, TowerOffset.Y, TowerOffset.Z);
	for(int i = 0; i < Key.Num(); i++)
	{
		bool bNextIsWater = i != Key.Num() - 1 && Key[i + 1] == 0;
		bool bPrevIsWater = i != 0 && Key[i - 1] == 0;
		bool bNextIsLand = i != Key.Num() - 1 && Key[i + 1] != 0;
		bool bPrevIsLand = i != 0 && Key[i - 1] != 0;
		if (Key[i] == 0)
		{
			if (bPrevIsLand){
				SpawnMesh(TEXT("Tower"), Tower, CurrentStartLocation + TowerOffset, FRotator(0.0), FVector(1.0));
				SpawnMesh(TEXT("Tower"), Tower, CurrentEndLocation + TowerOffset, FRotator(0.0), FVector(1.0));
				SpawnWalls(CurrentStartLocation - WallOffset, CurrentEndLocation + WallOffset, RandWallSelection);
			}
			CurrentStartLocation = CurrentEndLocation + DirectionNormal * Scale;
		}
		CurrentEndLocation += DirectionNormal * Scale;
	}
	SpawnMesh(TEXT("Tower"), Tower, CurrentStartLocation + TowerOffset, FRotator(0.0), FVector(1.0));
	SpawnWalls(CurrentStartLocation - WallOffset, EndPosition + WallOffset, RandWallSelection);
}

void AWallGenerator::SpawnWalls(FVector StartPosition, FVector EndPosition, int Type)
{
	float Distance = FVector::Distance(StartPosition, EndPosition);
	float Radians = FMath::Atan2(EndPosition.Y - StartPosition.Y, EndPosition.X - StartPosition.X);
	float Angle = FMath::RadiansToDegrees(Radians);
	FRotator Rotation = FRotator(0.0, Angle, 0.0);
	FVector WallDimensions = GetMeshDimensions(Walls[Type]);
	int NumOfWalls = Distance / WallDimensions.X;
	float MaxWallDistance = NumOfWalls * WallDimensions.X;
	float Spacer = (Distance - MaxWallDistance) / 2;
	FVector DirectionNormal = (EndPosition - StartPosition).GetSafeNormal();
	FVector CurrentLocation = StartPosition + (DirectionNormal * Spacer);
	UE_LOG(LogTemp, Display, TEXT("Distance: %f"), Distance);
	UE_LOG(LogTemp, Display, TEXT("Dimensions: %f"), WallDimensions.X);
	for (int i = 0; i < NumOfWalls; i++)
	{
		UE_LOG(LogTemp, Display, TEXT("Spawning Wall"))
		FVector SpawnLocation = FVector(CurrentLocation.X, CurrentLocation.Y, GetActorLocation().Z);
		SpawnMesh(TEXT("Wall"), Walls[Type], SpawnLocation, Rotation, FVector(1.0f));
		CurrentLocation += DirectionNormal * WallDimensions.X;
	}
}