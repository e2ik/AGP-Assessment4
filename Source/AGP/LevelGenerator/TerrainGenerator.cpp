// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"

#include "DiffUtils.h"
#include "HairStrandsInterface.h"
#include "LanternGenerator.h"
#include "SpawningNode.h"
#include "WallGenerator.h"
#include "Components/SplineComponent.h"

// Sets default values
ATerrainGenerator::ATerrainGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();
	BoatNavigator = GetWorld()->GetSubsystem<UBoatNavigation>();
	ProceduralNodes = GetWorld()->GetSubsystem<UProceduralNodes>();
	if (ProceduralNodes) { ProceduralNodes->GenerateNodes(); }
}

// Called every frame
void ATerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
	if (bShouldUpdate)
	{
		DeleteNodes();
		DeleteLanterns();
		CreateMap();
		GenerateTerrain();
		if (BoatNavigator) {
			BoatNavigator->PopulateNodes(Map, Height, Width, GetActorLocation(), GetMeshDimensions(FloorBlock));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO BOAT NAVIGATOR"))
			BoatNavigator = GetWorld()->GetSubsystem<UBoatNavigation>();
		}
		bShouldUpdate = false;
	}
	if (bShouldDelete)
	{
		DeleteMeshes();
		DeleteNodes();
		DeleteLanterns();
		bShouldDelete = false;
	}
	if (!BoatNavigator) {
		BoatNavigator = GetWorld()->GetSubsystem<UBoatNavigation>();
	}
	else if (BoatNavigator->HasNoNodes())
	{
		BoatNavigator->PopulateNodes(Map, Height, Width, GetActorLocation(), GetMeshDimensions(FloorBlock));
	}
}

void ATerrainGenerator::CreateMap()
{
	Map.Init(9, Height * Width);
	for(int y = 0; y < Height; y++)
	{
		for(int x = 0; x < Width; x++)
		{
			int32 rand = FMath::RandRange(0, 15);
			bool bNotAnEdge = x != 0 && y != 0;
			if (rand == 0)
			{
				Map[y * Width + x] = 0;
			}
			else if (bNotAnEdge)
			{
				bool bNextToWater = (Map[(y - 1) * Width + x] == 0 || Map[y * Width + x - 1] == 0) && Map[(y - 1) * Width + x - 1] != 0;
				if (bNextToWater) {
					Map[y * Width + x] = 0;
				} else
				{
					Map[y * Width + x] = 9;
				}
			}
			else
			{
				Map[y * Width + x] = 9;
			}
		}
	}
	WallGenerator->GenerateWalls(Width, Height, 3000.0, Map);
}


void ATerrainGenerator::GenerateTerrain()
{
	DeleteMeshes();
	FVector ActorLocation = GetActorLocation();
	FVector Dimensions = GetMeshDimensions(FloorBlock);
	for(int y = 0; y < Height; y++)
	{
		for(int x = 0; x < Width; x++)
		{
			if (Map[y * Width + x] != 0) {
				PlaceGround(x, y, Dimensions, ActorLocation);
			}
			else
			{
				PlaceWater(x, y, Dimensions, ActorLocation);
			}
		}
	}
	SpawnLanternPost();
	for (ASpawningNode* Node : LanternNodes) {
		for (ASpawningNode* OtherNode : Node->GetConnectedNodes()) {
			LanternGenerator->SpawnLanterns(Node->GetActorLocation(), OtherNode->GetActorLocation());
			GardenGenerator->SpawnFences(Node->GetActorLocation(), OtherNode->GetActorLocation(), false, 1);
			SpawnedMeshes.Append(GardenGenerator->SpawnedMeshes);
			GardenGenerator->SpawnedMeshes.Empty();
			SpawnedMeshes.Append(LanternGenerator->SpawnedMeshes);
			LanternGenerator->SpawnedMeshes.Empty();
		}
	}
}

void ATerrainGenerator::PlaceGround(int x, int y, FVector Dimensions, FVector ActorLocation)
{
	float PosX = (ActorLocation.X + (Dimensions.X * x)) - (Dimensions.X * (Width / 2));
	float PosY = (ActorLocation.Y + (Dimensions.Y * y)) - (Dimensions.Y * (Height / 2));
	FVector Location = FVector(PosX, PosY, 50.0);
	FRotator Rotation = FRotator(0.0, 0.0, 0.0);
	FVector Scale = FVector(1.0, 1.0, 1.0);
	SpawnMesh(TEXT("Ground_Big"), FloorBlock, Location, Rotation, Scale);
	int32 RandBuildingSelection = FMath::RandRange(0, 12);
	BuildingGenerator->SpawnBuildingByPreset(RandBuildingSelection, Location, Dimensions, GardenGenerator);
	SpawnedMeshes.Append(BuildingGenerator->SpawnedMeshes);
	BuildingGenerator->SpawnedMeshes.Empty();
}

void ATerrainGenerator::PlaceWater(int x, int y, FVector Dimensions, FVector ActorLocation)
{
	FVector WaterFloorLocation = FVector();
	WaterFloorLocation.X = (ActorLocation.X + (Dimensions.X * x)) - (Dimensions.X * (Width / 2));;
	WaterFloorLocation.Y = (ActorLocation.Y + (Dimensions.Y * y)) - (Dimensions.Y * (Height / 2));
	WaterFloorLocation.Z = -250;
	SpawnMesh(TEXT("Ground_Big"), FloorBlock, WaterFloorLocation, FRotator(0), FVector(1.0));
	float PosX = (ActorLocation.X + (Dimensions.X * x)) - (Dimensions.X * (Width / 2));
	float PosY = (ActorLocation.Y + (Dimensions.Y * y)) - (Dimensions.Y * (Height / 2));
	FVector Location = FVector(PosX, PosY, 50.0);
	FVector Scale = FVector(1.0, 1.0, 1.0);
	FRotator Rotation = FRotator(0.0, 90.0, 0.0);
	FVector BridgeScale = FVector(1.7, 1.7, 2.2);
	bool bLeft = x != 0 &&  Map[y * Width + x - 1] != 0;
	bool bRight = x != Width - 1 && Map[y * Width + x + 1] != 0;
	bool bDown = y != 0 && Map[(y - 1) * Width + x] != 0;
	bool bUp = y != Height - 1 && Map[(y + 1) * Width + x] != 0;
	
	if(bLeft) //Left
	{
		Location = FVector(PosX + 700.0, PosY, Location.Z);
		SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, Location, Rotation, Scale);
	}
	if(bRight) //Right
	{
		Location = FVector(PosX + 3000.0, PosY, Location.Z);
		SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, Location, Rotation, Scale);
	}
	if(bDown) //Down
	{
		Location = FVector(PosX, PosY, Location.Z);
		Rotation = FRotator(0.0, 0.0, 0.0);
		SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, Location, Rotation, Scale);
	}
	if(bUp) //Up
	{
		Location = FVector(PosX, PosY + 2300.0, Location.Z);
		Rotation = FRotator(0.0, 0.0, 0.0);
		SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, Location, Rotation, Scale);
	}
	//bridges should only spawn in a checkerboard pattern
	if (x % 2 == 0 && y % 2 == 0 || x % 2 == 1 && y % 2 == 1)
	{
		FVector BridgeDimensions = GetMeshDimensions(BridgeMesh) * BridgeScale;
		BridgeDimensions.Z = 0.0;
		if (bLeft && bRight)
		{
			FVector Offset = (Dimensions - BridgeDimensions) / 2;
			Offset.Z = 0.0;
			FVector BridgeLocation = FVector(PosX, PosY, 50.0) + Offset;
			Rotation = FRotator(0.0, 0.0, 0.0);
			SpawnMesh(TEXT("Bridge"), BridgeMesh, BridgeLocation, Rotation, BridgeScale);
		}
		else if (bUp && bDown)
		{
			FVector InvertBridge = FVector(-BridgeDimensions.Y, BridgeDimensions.X, 0.0);
			FVector Offset = (Dimensions - InvertBridge) / 2;
			Offset.Z = 0.0;
			FVector BridgeLocation = FVector(PosX, PosY, 50.0) + Offset;
			Rotation = FRotator(0.0, 90.0, 0.0);
			SpawnMesh(TEXT("Bridge"), BridgeMesh, BridgeLocation, Rotation, BridgeScale);
		}
	}
}


void ATerrainGenerator::SpawnLanternPost()
{
	FVector Scale = FVector(2.5, 2.0, 7.5);
	FVector PostDimensions = GetMeshDimensions(RiverPosts);
	FRotator Rotation = FRotator(0.0);
	FVector ActorLocation = GetActorLocation();
	FVector Dimensions = GetMeshDimensions(FloorBlock);
	float PostHeight = PostDimensions.Z * Scale.Z;
	TArray<FVector> GazeboLocations {};
	for (int Y = 0 ; Y < Height; Y++)
	{
		for (int X = 0; X < Width; X++)
		{
			UE_LOG(LogTemp, Display, TEXT("Spawining Post"));
			UE_LOG(LogTemp, Display, TEXT("%d/%d, %d/%d"), Y, Height, X, Width);
			if (Map[Y * Width + X] == 0)
			{
				float PosX = (ActorLocation.X + (Dimensions.X * X)) - (Dimensions.X * (Width / 2)) + Dimensions.X / 2;
				float PosY = (ActorLocation.Y + (Dimensions.Y * Y)) - (Dimensions.Y * (Height / 2)) + Dimensions.Y / 2;
				float Offset = 75.0;
				bool bUp = Y < Height - 1 ? Map[(Y + 1) * Width + X] == 0 : true;
				bool bDown = Y > 0 ? Map[(Y - 1) * Width + X] == 0 : true;
				bool bLeft = X < Width - 1 ? Map[Y * Width + X + 1] == 0 : true;
				bool bRight = X > 0 ? Map[Y * Width + X - 1] == 0 : true;
				bool bUL = Y < Height - 1 && X < Width - 1 ? Map[(Y + 1) * Width + X + 1] == 0 : true;
				bool bDL = Y > 0 && X < Width - 1 ? Map[(Y - 1) * Width + X + 1] == 0 : true;
				bool bUR = Y < Height - 1 && X > 0 ? Map[(Y + 1) * Width + X - 1] == 0 : true;
				bool bDR = Y > 0 && X > 0 ? Map[(Y - 1) * Width + X - 1] == 0 : true;
				TArray<FVector> Locations {};
				if (bUp && bDown && !bLeft && !bRight)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4, 50.0), //Upper Left Corner
						FVector(PosX - Dimensions.X / 4 - Offset, PosY + Dimensions.Y / 4, 50.0), //Upper Right Corner
						FVector(PosX + Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4, 50.0), //Lower Left Corner
						FVector(PosX - Dimensions.X / 4 - Offset, PosY - Dimensions.Y / 4, 50.0), //Lower Right Corner
					};
				}
				else if (bUp && bLeft && bRight && !bDown)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0), //Lower Left Corner
						FVector(PosX - Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0), //Lower Right Corner
					};
				}
				else if (bUp && bLeft && !bRight && bDown)
				{
					Locations = {
						FVector(PosX - Dimensions.X / 4 - Offset, PosY + Dimensions.Y / 4, 50.0), //Upper Right Corner
						FVector(PosX - Dimensions.X / 4 - Offset, PosY - Dimensions.Y / 4, 50.0), //Lower Right Corner
					};
				}
				else if (bUp && !bLeft && bRight && bDown)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4, 50.0), //Upper Right Corner
						FVector(PosX + Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4, 50.0), //Lower Right Corner
					};
				}
				else if (!bUp && bLeft && bRight && bDown)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0), //Lower Left Corner
						FVector(PosX - Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0), //Lower Right Corner
					};
				}
				else if (!bUp && !bDown && bLeft && bRight)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX - Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX + Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0),
						FVector(PosX - Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0),
					};
				}
				else if (bUp && bRight && !bLeft && !bDown)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4, 50.0),
						FVector(PosX + Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4 - Offset, 50.0),
						FVector(PosX - Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0),
					};
				}
				else if (bUp && bLeft && !bRight && !bDown)
				{
					Locations = {
						FVector(PosX - Dimensions.X / 4 - Offset, PosY + Dimensions.Y / 4, 50.0),
						FVector(PosX + Dimensions.X / 4 - Offset, PosY - Dimensions.Y / 4 - Offset, 50.0),
						FVector(PosX - Dimensions.X / 4, PosY - Dimensions.Y / 4 - Offset, 50.0),
					};
				}
				else if (bDown && bRight && !bLeft && !bUp)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX - Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX + Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4, 50.0),
					};
				}
				else if (bDown && bLeft && !bRight && ! bUp)
				{
					Locations = {
						FVector(PosX + Dimensions.X / 4, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX - Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4 + Offset, 50.0),
						FVector(PosX - Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4, 50.0),
					};
				}
				//Spawn Upper Right Corner Gazebo
				if (!bUR && bUp && bRight) {
					FVector GazeboLocation = FVector(PosX - Dimensions.X / 3, PosY + Dimensions.Y / 3, 50.0);
					SpawnMesh(TEXT("Gazebo"), Gazebo, GazeboLocation, FRotator(0.0, 45.0, 0.0), FVector(1.0));
					SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, FVector(PosX - 1500.0, PosY + 800.0, 50.0), FRotator(0.0), FVector(700.0f/3000.0f, 1.0, 1.0));
					GazeboLocations.Add(FVector(PosX - Dimensions.X / 4 - Offset, PosY + Dimensions.Y / 4 + Offset + 700.0, 50.0));
					GazeboLocations.Add(FVector(PosX - Dimensions.X / 4 - Offset - 700.0, PosY + Dimensions.Y / 4 + Offset, 50.0));
				}
				//Spawn Upper Left Corner Gazebo
				if (!bUL && bUp && bLeft)
				{
					FVector GazeboLocation = FVector(PosX + Dimensions.X / 3, PosY + Dimensions.Y / 3, 50.0);
					SpawnMesh(TEXT("Gazebo"), Gazebo, GazeboLocation, FRotator(0.0, 315.0, 0.0), FVector(1.0));
					SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, FVector(PosX + 800.0, PosY + 800.0, 50.0), FRotator(0.0), FVector(700.0f/3000.0f, 1.0, 1.0));
					GazeboLocations.Add(FVector(PosX + Dimensions.X / 4 + Offset, PosY + Dimensions.Y / 4 + Offset + 700.0, 50.0));
					GazeboLocations.Add(FVector(PosX + Dimensions.X / 4 + Offset + 700.0, PosY + Dimensions.Y / 4 + Offset, 50.0));
				}
				//Spawn Lower Right Corner Gazebo
				if (!bDR && bDown && bRight) {
					FVector GazeboLocation = FVector(PosX - Dimensions.X / 3, PosY - Dimensions.Y / 3, 50.0);
					SpawnMesh(TEXT("Gazebo"), Gazebo, GazeboLocation, FRotator(0.0, 135.0, 0.0), FVector(1.0));
					SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, FVector(PosX - 1500.0, PosY - 1500.0, 50.0), FRotator(0.0), FVector(700.0f/3000.0f, 1.0, 1.0));
					GazeboLocations.Add(FVector(PosX - Dimensions.X / 4 - Offset - 700.0, PosY - Dimensions.Y / 4 - Offset, 50.0));
					GazeboLocations.Add(FVector(PosX - Dimensions.X / 4 - Offset, PosY - Dimensions.Y / 4 - Offset - 700.0, 50.0));
				}
				//Spawn Lower Left Corner Gazebo
				if (!bDL && bDown && bLeft)
				{
					FVector GazeboLocation = FVector(PosX + Dimensions.X / 3, PosY - Dimensions.Y / 3, 50.0);
					SpawnMesh(TEXT("Gazebo"), Gazebo, GazeboLocation, FRotator(0.0, 225.0, 0.0), FVector(1.0));
					SpawnMesh(TEXT("Ground_Narrow"), FloorBlock2, FVector(PosX + 800.0, PosY - 1500.0, 50.0), FRotator(0.0), FVector(700.0f/3000.0f, 1.0, 1.0));
					GazeboLocations.Add(FVector(PosX + Dimensions.X / 4 + Offset + 700.0, PosY - Dimensions.Y / 4 - Offset, 50.0));
					GazeboLocations.Add(FVector(PosX + Dimensions.X / 4 + Offset, PosY - Dimensions.Y / 4 - Offset - 700.0, 50.0));
				}
				for (FVector Location : Locations)
				{
					SpawnMesh(TEXT("RiverPost"), RiverPosts, Location, Rotation, Scale);
					ASpawningNode* NewNode = GetWorld()->SpawnActor<ASpawningNode>();
					NewNode->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + PostHeight));
					LanternNodes.Add(NewNode);
					for(ASpawningNode* Node : LanternNodes)
					{
						float Distance = FVector::Distance(Node->GetActorLocation(), NewNode->GetActorLocation());
						bool bMaxDistanceA = Dimensions.X / 2 + 100.0f >= Distance;
						bool bMinDistanceA = Dimensions.X / 2 - 100.0f <= Distance;
						
						if ((bMaxDistanceA && bMinDistanceA))
						{
							NewNode->ConnectNodes(Node);
						}
					}
				}
			}
		}
	}
	for (FVector Location : GazeboLocations)
	{
		ASpawningNode* NewNode = GetWorld()->SpawnActor<ASpawningNode>();
		NewNode->SetActorLocation(FVector(Location.X, Location.Y, Location.Z + PostHeight));
		for (ASpawningNode* Node : LanternNodes)
		{
			float Distance = FVector::Distance(NewNode->GetActorLocation(), Node->GetActorLocation());
			bool bMaxDistanceA = Dimensions.X / 4 + 50.0f >= Distance;
			bool bMinDistanceA = Dimensions.X / 4 - 50.0f <= Distance;
			if (bMaxDistanceA && bMinDistanceA)
			{
				NewNode->ConnectNodes(Node);
			}
		}
		LanternNodes.Add(NewNode);
	}
}

void ATerrainGenerator::DeleteNodes()
{
	if (!LanternNodes.IsEmpty())
	{
		for(auto Node : LanternNodes)
		{
			GetWorld()->DestroyActor(Node);
		}
		LanternNodes.Empty();
	}
}

void ATerrainGenerator::DeleteLanterns()
{
	
}
