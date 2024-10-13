// Fill out your copyright notice in the Description page of Project Settings.


#include "GardenGenerator.h"

#include "SpawningNode.h"

class AStaticMeshActor;
// Sets default values
AGardenGenerator::AGardenGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGardenGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGardenGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldRegenerate)
	{
		DeleteMeshes();
		SpawnGarden();
		bShouldRegenerate = false;
	}
	if (bShouldDelete)
	{
		DeleteMeshes();
		bShouldDelete = false;
	}
	if (bShouldSpawnFences)
	{
		DeleteMeshes();
		SpawnFences(FenceStart->GetActorLocation(), FenceEnd->GetActorLocation(), false, 0);
		bShouldSpawnFences = false;
	}
	if (bShouldGenerateLShapedGarden)
	{
		DeleteMeshes();
		SpawnLGarden(OmitQuadrant);
		bShouldGenerateLShapedGarden = false;
	}
}

void AGardenGenerator::SpawnGarden()
{
	float MaxWidth = Width * Dimensions;
	float MaxHeight = Height * Dimensions;
	int TreeType = FMath::RandRange(0, 2);
	int FenceType = FMath::RandRange(0, 2);
	int GrassType = FMath::RandRange(0, 1);
	FVector Location = GetActorLocation();
	FRotator Rotation = FRotator(0, 0, 0);
	FVector Scale = FVector(1.0, 1.0, 1.0);
	FVector GrassScale = FVector(float(Width) / 2, float(Height) / 2, 1.0);
	SpawnMesh(TEXT("Grass"), Grass[GrassType], Location, Rotation, GrassScale);
	if (FenceType < 2) {
		FVector FenceDimensions = GetMeshDimensions(Fences[FenceType]);
		int NumFencesWidth = MaxWidth / FenceDimensions.X;
		int NumFencesHeight = MaxHeight / FenceDimensions.X;
		auto PosX = Location.X - ((FenceDimensions.X * NumFencesWidth) / 2);
		auto PosY = Location.Y - ((FenceDimensions.X * NumFencesHeight) / 2);
		bool bSpawnEntrance = FenceType == 1;
		for (int i = 0; i < 4; i++)
		{
			double RotY = (i * 90);
			int Max = i % 2 == 0 ? NumFencesWidth : NumFencesHeight;
			for (int j = 0; j < Max; j++)
			{
				FVector FencePosition = FVector(PosX, PosY, Location.Z);
				FRotator FenceRotation = FRotator(0, RotY, 0);
				if (j != Max / 2 || bSpawnEntrance || Width < 2 || Height < 2)
				{
					SpawnMesh(TEXT("Fence"), Fences[FenceType], FencePosition, FenceRotation, Scale);
				}
				if (i % 2 == 0)
				{
					PosX += i == 0 ? FenceDimensions.X : -FenceDimensions.X;
				} else
				{
					PosY += i == 1 ? FenceDimensions.X : -FenceDimensions.X;
				}
			}
		}
	}
	if (TreeType < Trees.Num())
	{
		FVector TreeDimension = GetMeshDimensions(Trees[TreeType]);
		int SpawnTreesX = MaxWidth / (TreeDimension.X * 0.75);
		int SpawnTreesY = MaxHeight / (TreeDimension.Y * 0.75);
		float SpacerX = (MaxWidth - (TreeDimension.X * SpawnTreesX)) / (SpawnTreesX + 1);
		float SpacerY = (MaxHeight - (TreeDimension.Y * SpawnTreesY)) / (SpawnTreesY + 1);
		float FactorX = TreeDimension.X + SpacerX;
		float FactorY = TreeDimension.Y + SpacerY;
		for (float y = 0; y < SpawnTreesY; ++y)
		{
			for (float x = 0; x < SpawnTreesX; ++x)
			{
				FRotator TreeRotation = FRotator(0, FMath::RandRange(0, 360), 0);
				float TreeX = SpacerX + Location.X - (MaxWidth / 2) + ((TreeDimension.X) / 2) + FactorX * x;
				float TreeY = SpacerY + Location.Y - (MaxHeight / 2) + ((TreeDimension.X) / 2) + FactorY * y;
				FVector TreeLocation = FVector(TreeX, TreeY, Location.Z);
				SpawnMesh(TEXT("Tree"), Trees[TreeType], TreeLocation, TreeRotation, Scale);
			}
		}
	}
}

void AGardenGenerator::SpawnLGarden(int Quadrant)
{
	UE_LOG(LogTemp, Display, TEXT("%d"), Quadrant);
	FVector ActorLocation = GetActorLocation();
	float MaxWidth = Width * Dimensions;
	float MaxHeight = Height * Dimensions;
	int TreeType = FMath::RandRange(0, 1);
	int FenceType = FMath::RandRange(0, 1);

	//Set the basic position parameters
	FVector Scale = FVector(1.0);
	FVector HalfDimensions = FVector(float(Width)/2 * Dimensions, float(Height)/2 * Dimensions, 0.0);
	FVector Offset = ActorLocation - HalfDimensions;

	//Offset fences so they connect
	FVector FenceDimensions = GetMeshDimensions(Fences[FenceType]);
	int MaxFencesW = float(MaxWidth) / 2 / FenceDimensions.X;
	int MaxFencesH = float(MaxHeight) / 2 / FenceDimensions.X;
	float MaxFencesWidth = MaxFencesW * FenceDimensions.X;
	float MaxFencesHeight = MaxFencesH * FenceDimensions.X;
	float OffsetXFences = ((MaxWidth / 2) - MaxFencesWidth) / 4;
	float OffsetYFences = ((MaxHeight / 2) - MaxFencesHeight) / 4;
	UE_LOG(LogTemp, Display, TEXT("%f"), OffsetXFences);
	
	TArray<FVector> BoundingPoints = {
		FVector(0.0 + OffsetXFences, 0.0 + OffsetYFences, 0.0) + Offset,
		FVector(float(Width)/2 * Dimensions, 0.0 + OffsetYFences, 0.0) + Offset,
		FVector((Width * Dimensions) - OffsetXFences, 0.0 + OffsetYFences, 0.0) + Offset,
		FVector(0.0 + OffsetXFences, float(Height)/2 * Dimensions, 0.0) + Offset,
		FVector(float(Width)/2 * Dimensions, float(Height)/2 * Dimensions, 0.0) + Offset,
		FVector((Width * Dimensions) - OffsetXFences, float(Height)/2 * Dimensions, 0.0) + Offset,
		FVector(0.0 + OffsetXFences, (Height * Dimensions) - OffsetYFences, 0.0) + Offset,
		FVector(float(Width)/2 * Dimensions, (Height * Dimensions) - OffsetYFences, 0.0) + Offset,
		FVector((Width * Dimensions) - OffsetXFences, (Height * Dimensions) - OffsetYFences, 0.0) + Offset,
	};

	FVector OffsetPost = GetMeshDimensions(FencePosts[FenceType]) / 2;
	OffsetPost.Z = 0;
	for (int i = 0; i < BoundingPoints.Num(); i++)
	{
		if (Quadrant == 1 && i == 8)
		{
			continue;
		}
		if (Quadrant == 2 && i == 6)
		{
			continue;
		}
		if (Quadrant == 3 && i == 0)
		{
			continue;
		}
		if (Quadrant == 4 && i == 2)
		{
			continue;
		}
		SpawnMesh(TEXT("FencePost"), FencePosts[FenceType], FenceType == 0 ? BoundingPoints[i] : BoundingPoints[i] + OffsetPost, FRotator(0), FenceType == 1 ? FVector(2.5, 2.0, 1.5) : FVector(1.5, 1.5, 1.0));
	}

	if (Quadrant == 1)
	{
		SpawnFences(BoundingPoints[5], BoundingPoints[4], false, FenceType);
		SpawnFences(BoundingPoints[4], BoundingPoints[7], false, FenceType);
	}
	else
	{
		SpawnFences(BoundingPoints[5], BoundingPoints[8], false, FenceType);
		SpawnFences(BoundingPoints[8], BoundingPoints[7], false, FenceType);
	}
	if (Quadrant == 2)
	{
		SpawnFences(BoundingPoints[7], BoundingPoints[4], false, FenceType);
		SpawnFences(BoundingPoints[4], BoundingPoints[3], false, FenceType);
	}
	else
	{
		SpawnFences(BoundingPoints[7], BoundingPoints[6], false, FenceType);
		SpawnFences(BoundingPoints[6], BoundingPoints[3], false, FenceType);
	}
	if (Quadrant == 3)
	{
		SpawnFences(BoundingPoints[3], BoundingPoints[4], false, FenceType);
		SpawnFences(BoundingPoints[4], BoundingPoints[1], false, FenceType);
	}
	else
	{
		SpawnFences(BoundingPoints[0], BoundingPoints[1], false, FenceType);
		SpawnFences(BoundingPoints[3], BoundingPoints[0], false, FenceType);
	}
	if (Quadrant == 4)
	{
		SpawnFences(BoundingPoints[1], BoundingPoints[4], false, FenceType);
		SpawnFences(BoundingPoints[4], BoundingPoints[5], false, FenceType);
	}
	else
	{
		SpawnFences(BoundingPoints[1], BoundingPoints[2], false, FenceType);
		SpawnFences(BoundingPoints[2], BoundingPoints[5], false, FenceType);
	}

	if (TreeType < Trees.Num())
	{
		FVector TreeDimension = GetMeshDimensions(Trees[TreeType]);
		int SpawnTreesX = MaxWidth / (TreeDimension.X * 0.75);
		int SpawnTreesY = MaxHeight / (TreeDimension.Y * 0.75);
		float SpacerX = (MaxWidth - (TreeDimension.X * SpawnTreesX)) / (SpawnTreesX + 1);
		float SpacerY = (MaxHeight - (TreeDimension.Y * SpawnTreesY)) / (SpawnTreesY + 1);
		float FactorX = TreeDimension.X + SpacerX;
		float FactorY = TreeDimension.Y + SpacerY;
		for (float y = 0; y < SpawnTreesY; ++y)
		{
			for (float x = 0; x < SpawnTreesX; ++x)
			{
				FRotator TreeRotation = FRotator(0, FMath::RandRange(0, 360), 0);
				float TreeX = SpacerX + ActorLocation.X - (MaxWidth / 2) + ((TreeDimension.X) / 2) + FactorX * x;
				float TreeY = SpacerY + ActorLocation.Y - (MaxHeight / 2) + ((TreeDimension.X) / 2) + FactorY * y;
				FVector TreeLocation = FVector(TreeX, TreeY, ActorLocation.Z);
				if (Quadrant != 1 && TreeX > BoundingPoints[4].X && TreeY > BoundingPoints[4].Y)
				{
					SpawnMesh(TEXT("Tree"), Trees[TreeType], TreeLocation, TreeRotation, Scale);
				}
				if (Quadrant != 2 && TreeX < BoundingPoints[4].X && TreeY > BoundingPoints[4].Y)
				{
					SpawnMesh(TEXT("Tree"), Trees[TreeType], TreeLocation, TreeRotation, Scale);
				}
				if (Quadrant != 3 && TreeX < BoundingPoints[4].X && TreeY < BoundingPoints[4].Y)
				{
					SpawnMesh(TEXT("Tree"), Trees[TreeType], TreeLocation, TreeRotation, Scale);
				}
				if (Quadrant != 4 && TreeX > BoundingPoints[4].X && TreeY < BoundingPoints[4].Y)
				{
					SpawnMesh(TEXT("Tree"), Trees[TreeType], TreeLocation, TreeRotation, Scale);
				}
			}
		}
	}
}

void AGardenGenerator::SpawnFences(FVector Start, FVector End, bool AddEntry, int FenceType)
{
	if (FenceType > Fences.Num() - 1)
	{
		UE_LOG(LogTemp, Display, TEXT("INVALID FENCE TYPE"));
	}
	float Distance = FVector::Distance(Start, End);
	float Radians = FMath::Atan2(End.Y - Start.Y, End.X - Start.X);
	float Angle = FMath::RadiansToDegrees(Radians);
	FRotator Rotation = FRotator(0.0, Angle, 0.0);
	FVector FenceDimensions = GetMeshDimensions(Fences[FenceType]);
	int NumOfFences = Distance / FenceDimensions.X;
	float MaxFenceDistance = NumOfFences * FenceDimensions.X;
	float Spacer = (Distance - MaxFenceDistance) / 2;
	FVector DirectionNormal = (End - Start).GetSafeNormal();
	FVector CurrentLocation = Start + DirectionNormal * Spacer;
	for (int i = 0; i < NumOfFences; i++)
	{
		if (!AddEntry || i != NumOfFences / 2) {
			FVector SpawnLocation = FVector(CurrentLocation.X, CurrentLocation.Y, GetActorLocation().Z);
			SpawnMesh(TEXT("Fence"), Fences[FenceType], SpawnLocation, Rotation, FVector(1.0f));
			CurrentLocation += DirectionNormal * FenceDimensions.X;
		}
	}
}

void AGardenGenerator::SetGardenParams(int NewHeight, int NewWidth)
{
	Height = NewHeight;
	Width = NewWidth;
}
