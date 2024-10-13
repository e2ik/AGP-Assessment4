// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingGenerator.h"

#include "GardenGenerator.h"
#include "IAutomationControllerManager.h"
#include "LanternGenerator.h"
#include "Chaos/KinematicTargets.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Systems/WeightAndEasingEvaluatorSystem.h"

// Sets default values
ABuildingGenerator::ABuildingGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABuildingGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABuildingGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bShouldGenerateBuilding)
	{
		BuildBuilding();
		bShouldGenerateBuilding = false;
	}
}

void ABuildingGenerator::BuildBuilding()
{
	//Set some derived attributes to be used in the loops
	int WallIndex = FMath::RandRange(0, 2);
	FVector ActorPosition = GetActorLocation();
	FVector Scale = FVector(1.0, 1.0, 1.0);
	double RoofRotY = Width > Depth ? 0 : 90;
	FRotator RoofRotation = FRotator(0, RoofRotY, 0);
	if(RoofIndex > Roofs.Num() - 1)
	{
		return;
	}
	DeleteMeshes();
	DeleteNodes();
	auto WallDimensions = GetMeshDimensions(Wall[WallIndex]);
	auto RoofDimensions = GetMeshDimensions(Roofs[RoofIndex]);
	float LevelHeight = WallDimensions.Z + RoofDimensions.Z + FloorOffset;
	for(float PosZ = ActorPosition.Z; PosZ < (Height * LevelHeight) + ActorPosition.Z; PosZ += LevelHeight ) {
		//Centers the building on the Gizmo
		auto PosX = ActorPosition.X - ((WallDimensions.X * Width) / 2);
		auto PosY = ActorPosition.Y - ((WallDimensions.X * Depth) / 2);
		for (int i = 0; i < 4; i++)
		{
			double RotY = i * 90;
			int Max = i % 2 == 0 ? Width : Depth;
			for (int j = 0; j < Max; j++)
			{
				if (i % 2 == 0)
				{
					PosX += i == 0 ? WallDimensions.X : -WallDimensions.X;
				} else
				{
					PosY += i == 1 ? WallDimensions.X : -WallDimensions.X;
				}
				FVector Position = FVector(PosX, PosY, PosZ);
				FRotator Rotation = FRotator(0, RotY, 0);
				if (j == Max / 2)
				{
					SpawnMesh(Door, Position, Rotation, Scale);
				} else
				{
					SpawnMesh(Wall[WallIndex], Position, Rotation, Scale);
				}
			}
		}
		float RoofPosX = ActorPosition.X;
		float RoofPosY = ActorPosition.Y;
		FVector RoofPos = FVector(RoofPosX, RoofPosY, WallDimensions.Z + PosZ + RoofOffset);
		SpawnMesh(Roofs[RoofIndex], RoofPos, RoofRotation, Scale);
		SpawnLanternNodes(RoofPos, RoofDimensions, Depth > Width);
	}

	//3 variables to correctly set the change in roof position
	float PosX = ActorPosition.X;
	float PosY = ActorPosition.Y;
	float PosZ = ActorPosition.Z + LevelHeight * Height;
	for(int r = 1; r < ExtraRoofs; r++)
	{
		Scale = Scale * 0.5;
		float e = FMath::Pow(2.0, float(r));
		SpawnMesh(Roofs[RoofIndex], FVector(PosX, PosY, PosZ), RoofRotation, Scale);
		PosZ = PosZ + RoofDimensions.Z / FMath::Pow(2.0, float(r));
	}
	if (0 == FMath::RandRange(0, 3))
	{
		SpawnLanterns();
	}
}

void ABuildingGenerator::SpawnLanternNodes(const FVector& Location, const FVector& Dimensions, bool bRotate)
{
	ASpawningNode* RoofNode1 = GetWorld()->SpawnActor<ASpawningNode>();
	ASpawningNode* RoofNode2 = GetWorld()->SpawnActor<ASpawningNode>();
	ASpawningNode* RoofNode3 = GetWorld()->SpawnActor<ASpawningNode>();
	ASpawningNode* RoofNode4 = GetWorld()->SpawnActor<ASpawningNode>();
	if (bRotate)
	{
		RoofNode1->SetActorLocation((Location + FVector(Dimensions.Y / 2 - LanternOffset.Y, Dimensions.X / 2 - LanternOffset.X, LanternOffset.Z)));
		RoofNode2->SetActorLocation(Location + FVector(-Dimensions.Y / 2 + LanternOffset.Y, -Dimensions.X / 2 + LanternOffset.X, LanternOffset.Z));
		RoofNode3->SetActorLocation(Location + FVector(Dimensions.Y / 2 - LanternOffset.Y, -Dimensions.X / 2 + LanternOffset.X, LanternOffset.Z));
		RoofNode4->SetActorLocation(Location + FVector(-Dimensions.Y / 2 + LanternOffset.Y, Dimensions.X / 2 - LanternOffset.X, LanternOffset.Z));
	}
	else
	{
		RoofNode1->SetActorLocation(Location + FVector(Dimensions.X / 2 - LanternOffset.X, Dimensions.Y / 2 - LanternOffset.Y, LanternOffset.Z));
		RoofNode2->SetActorLocation(Location + FVector(-Dimensions.X / 2 + LanternOffset.X, -Dimensions.Y / 2 + LanternOffset.Y, LanternOffset.Z));
		RoofNode3->SetActorLocation(Location + FVector(Dimensions.X / 2 - LanternOffset.X, -Dimensions.Y / 2 + LanternOffset.Y, LanternOffset.Z));
		RoofNode4->SetActorLocation(Location + FVector(-Dimensions.X / 2 + LanternOffset.X, Dimensions.Y / 2 - LanternOffset.Y, LanternOffset.Z));
	}
	RoofNode1->ConnectNodes(RoofNode3);
	RoofNode2->ConnectNodes(RoofNode4);
	RoofNode3->ConnectNodes(RoofNode2);
	RoofNode4->ConnectNodes(RoofNode1);
	LanternNodes.Append({RoofNode1, RoofNode2, RoofNode3, RoofNode4});
}


void ABuildingGenerator::SpawnBuildingByPreset(int Preset, FVector Location, FVector Dimensions, AGardenGenerator* GardenGenerator)
{
	//Buildings 0 - 3 are double wides.
	int32 BuildingHeight = FMath::RandRange(1, 20);
	bool bBuildingRotation = FMath::RandBool();
	BuildingHeight = FMath::Sqrt(float(BuildingHeight));
	float XOffset = 0;
	float YOffset = 0;
	int W = 3;
	int D = 3;
	int Q = 0;
	bool bShouldSpawnGarden = false;
	
	switch (Preset)
	{
	case 0:
		W = bBuildingRotation ? 8 : 3;
		D = bBuildingRotation ? 3 : 8;
		XOffset = bBuildingRotation ? 0 : 750;
		YOffset = bBuildingRotation ? 750 : 0;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, 1, 1, 0, 0.0, 0.0);
		break;
	case 1:
		W = bBuildingRotation ? 8 : 3;
		D = bBuildingRotation ? 3 : 8;
		XOffset = bBuildingRotation ? 0 : 750;
		YOffset = bBuildingRotation ? 750 : 0;
		LanternOffset = FVector(100, 100, 100.0);
		SetBuildingGeneratorParameters(W, D, 1, 1, 1, -100.0, 0.0);
		break;
	case 2:
		W = bBuildingRotation ? 8 : 3;
		D = bBuildingRotation ? 3 : 8;
		XOffset = bBuildingRotation ? 0 : 750;
		YOffset = bBuildingRotation ? 750 : 0;
		LanternOffset = FVector(100, 100, 100.0);
		SetBuildingGeneratorParameters(W, D, 1, 1, 2, -100.0, 0.0);
		break;
	case 3:
		W = bBuildingRotation ? 7 : 3;
		D = bBuildingRotation ? 3 : 7;
		XOffset = bBuildingRotation ? 0 : 750;
		YOffset = bBuildingRotation ? 750 : 0;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 1, 3, 0.0, -50.0);
		break;
	case 4:
		if (BuildingHeight == 1)
		{
			XOffset = -750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 3;
		}
		if (BuildingHeight == 2)
		{
			XOffset = 750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 4;
		}
		if (BuildingHeight == 3)
		{
			XOffset = -750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 2;
		}
		if (BuildingHeight == 4)
		{
			XOffset = 750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 1;
		}
		LanternOffset = FVector(25.0, 25.0, 12.0);
		SetBuildingGeneratorParameters(3, 3, 1, 1, 4, 0.0, 0.0);
		break;
	case 5:
		if (BuildingHeight == 1)
		{
			XOffset = -750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 3;
		}
		if (BuildingHeight == 2)
		{
			XOffset = 750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 4;
		}
		if (BuildingHeight == 3)
		{
			XOffset = -750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 2;
		}
		if (BuildingHeight == 4)
		{
			XOffset = 750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 1;
		}
		LanternOffset = FVector(25.0, 25.0, 12.0);
		SetBuildingGeneratorParameters(3, 3, 1, 1, 5, 0.0, 0.0);
		break;
	case 6:
		W = bBuildingRotation ? 4 : 5;
		D = bBuildingRotation ? 5 : 4;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 6, 0.0, 0.0);
		break;
	case 7:
		if (BuildingHeight == 1)
		{
			XOffset = -750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 3;
		}
		if (BuildingHeight == 2)
		{
			XOffset = 750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 4;
		}
		if (BuildingHeight == 3)
		{
			XOffset = -750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 2;
		}
		if (BuildingHeight == 4)
		{
			XOffset = 750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 1;
		}
		W = bBuildingRotation ? 2 : 3;
		D = bBuildingRotation ? 3 : 2;
		LanternOffset = FVector(100, 100, 150.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 7, 0.0, 0.0);
		break;
	case 8:
		W = bBuildingRotation ? 3 : 4;
		D = bBuildingRotation ? 4 : 3;
		LanternOffset = FVector(0.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 8, 0.0, 0.0);
		break;
	case 9:
		W = bBuildingRotation ? 3 : 4;
		D = bBuildingRotation ? 4 : 3;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 9, 0.0, 0.0);
		break;
	case 10:
		W = bBuildingRotation ? 3 : 4;
		D = bBuildingRotation ? 4 : 3;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 10, 0.0, 0.0);
		break;
	case 11:
		W = bBuildingRotation ? 4 : 5;
		D = bBuildingRotation ? 5 : 4;
		LanternOffset = FVector(0.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 11, 0.0, 0.0);
		break;
	case 12:
		if (BuildingHeight == 1)
		{
			XOffset = -750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 3;
		}
		if (BuildingHeight == 2)
		{
			XOffset = 750;
			YOffset = -750;
			bShouldSpawnGarden = true;
			Q = 4;
		}
		if (BuildingHeight == 3)
		{
			XOffset = -750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 2;
		}
		if (BuildingHeight == 4)
		{
			XOffset = 750;
			YOffset = 750;
			bShouldSpawnGarden = true;
			Q = 1;
		}
		W = bBuildingRotation ? 2 : 3;
		D = bBuildingRotation ? 3 : 2;
		LanternOffset = FVector(100, 100, 50.0);
		SetBuildingGeneratorParameters(W, D, BuildingHeight, 3, 12, 0.0, 0.0);
		break;
	default:
		SetBuildingGeneratorParameters(3, 3, 1, 1, 5, 0.0, 0.0);
		break;
	}
	if (FMath::RandBool())
	{
		XOffset *= -1;
		YOffset *= -1;
		Q = Q > 2 ? Q - 2 : Q + 2;
	}
	FVector NewActorLocation = FVector(Location.X + (Dimensions.X / 2) + XOffset, Location.Y + (Dimensions.Y / 2) + YOffset, Location.Z);
	SetActorLocation(NewActorLocation);
	BuildBuilding();
	if (0 < Preset && Preset < 4)
	{
		bool bConnectGarden = FMath::RandBool();
		if (bConnectGarden)
		{
			//Spawn one garden if connected
			float GardenX = Location.X + (Dimensions.X / 2) - XOffset;
			float GardenY = Location.Y + (Dimensions.Y / 2) - YOffset;
			FVector GardenLocation = FVector(GardenX, GardenY, Location.Z);
			int GardenWidth = bBuildingRotation ? 2 : 5;
			int GardenHeigh = bBuildingRotation ? 5 : 2;
			GardenGenerator->SetActorLocation(GardenLocation);
			GardenGenerator->SetGardenParams(GardenWidth, GardenHeigh);
			GardenGenerator->SpawnGarden();
			SpawnedMeshes.Append(GardenGenerator->SpawnedMeshes);
			GardenGenerator->SpawnedMeshes.Empty();
		} else
		{
			//Spawn 2 gardens if the gardens arent connected.
			float GardenOneX;
			float GardenTwoX;
			float GardenOneY;
			float GardenTwoY;
			if (XOffset != 0)
			{
				GardenOneX = XOffset * -1;
				GardenTwoX = XOffset * -1;
				GardenOneY = -750;
				GardenTwoY = 750;
			} else
			{
				GardenOneX = -750;
				GardenTwoX = 750;
				GardenOneY = YOffset * -1;
				GardenTwoY = YOffset * -1;
			}
			float GardenX = Location.X + (Dimensions.X / 2) + GardenOneX;
			float GardenY = Location.Y + (Dimensions.Y / 2) + GardenOneY;
			FVector GardenLocation = FVector(GardenX, GardenY, Location.Z);
			GardenGenerator->SetActorLocation(GardenLocation);
			GardenGenerator->SetGardenParams(2, 2);
			GardenGenerator->SpawnGarden();
			SpawnedMeshes.Append(GardenGenerator->SpawnedMeshes);
			GardenGenerator->SpawnedMeshes.Empty();
			GardenX = Location.X + (Dimensions.X / 2) + GardenTwoX;
			GardenY = Location.Y + (Dimensions.Y / 2) + GardenTwoY;
			GardenLocation = FVector(GardenX, GardenY, Location.Z);
			GardenGenerator->SetActorLocation(GardenLocation);
			GardenGenerator->SetGardenParams(2, 2);
			GardenGenerator->SpawnGarden();
			SpawnedMeshes.Append(GardenGenerator->SpawnedMeshes);
			GardenGenerator->SpawnedMeshes.Empty();
		}
	}
	else if (TArray<int> {4, 5, 7, 12}.Contains(Preset))
	{
		float GardenX = Location.X + (Dimensions.X / 2);
		float GardenY = Location.Y + (Dimensions.Y / 2);
		FVector GardenLocation = FVector(GardenX, GardenY, Location.Z);
		GardenGenerator->SetActorLocation(GardenLocation);
		GardenGenerator->SetGardenParams(5, 5);
		GardenGenerator->SpawnLGarden(Q);
		SpawnedMeshes.Append(GardenGenerator->SpawnedMeshes);
		GardenGenerator->SpawnedMeshes.Empty();
	}
}

void ABuildingGenerator::SetBuildingGeneratorParameters(int W, int D, int H, int XRoofs, int RIndex, float ROffset, float FOffset)
{
	Width = W;
	Depth = D;
	Height = H;
	ExtraRoofs = XRoofs;
	RoofIndex = RIndex;
	RoofOffset = ROffset;
	FloorOffset = FOffset;
}

void ABuildingGenerator::DeleteNodes()
{
	if (!LanternNodes.IsEmpty())
	{
		for (auto Node : LanternNodes)
		{
			GetWorld()->DestroyActor(Node);
		}
		LanternNodes.Empty();
	}
}

void ABuildingGenerator::SpawnLanterns()
{
	for (ASpawningNode* Node : LanternNodes) {
		for (ASpawningNode* OtherNode : Node->GetConnectedNodes()) {
			LanternGenerator->SpawnLanterns(Node->GetActorLocation(), OtherNode->GetActorLocation());
			SpawnedMeshes.Append(LanternGenerator->SpawnedMeshes);
			LanternGenerator->SpawnedMeshes.Empty();
		}
	}
}
