// Fill out your copyright notice in the Description page of Project Settings.


#include "BoatNavigation.h"

#include "Boat.h"

void UBoatNavigation::OnWorldBeginPlay(UWorld& InWorld)
{
	
}


void UBoatNavigation::PopulateNodes(TArray<int> Map, int Height, int Width, FVector Location, FVector Dimensions)
{
	if (!Nodes.IsEmpty())
	{
		for (int i = 0; i < Nodes.Num(); i++)
		{
			GetWorld()->DestroyActor(Nodes[i]);
		}
		Nodes.Empty();
	}
	if (Map.IsEmpty()) return; 
	for (int Y = 0; Y < Height; Y++)
	{
		for (int X = 0; X < Width; X++)
		{
			if (Map[Y * Width + X] == 0) {
				float PosX = (Location.X + (Dimensions.X * X)) - (Dimensions.X * (Width / 2)) + Dimensions.X / 2;
				float PosY = (Location.Y + (Dimensions.Y * Y)) - (Dimensions.Y * (Height / 2)) + Dimensions.Y / 2;
				ABoatNode* BoatNode = GetWorld()->SpawnActor<ABoatNode>();
				BoatNode->SetActorLocation(FVector(PosX, PosY, 50.0));
				if (Y > 0 && Map[(Y - 1) * Width + X] == 0)
				{
					BoatNode->ConnectNodes(Nodes[(Y - 1) * Width + X]);
				}
				if (X > 0 && Map[Y * Width + X - 1] == 0)
				{
					BoatNode->ConnectNodes(Nodes[Y * Width + X - 1]);
				}
				Nodes.Add(BoatNode);
			}
			else
			{
				Nodes.Add(nullptr);
			}
		}
	}
	for (auto Node : Nodes)
	{
		if (!Node)
		{
			Nodes.Remove(Node);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Populated Nodes = %d"), Nodes.Num());
}

ABoatNode* UBoatNavigation::GetNewBoatPath()
{
	TArray<ABoatNode*> PossibleNodes {};
	for(ABoatNode* Node : Nodes)
	{
		if (Node->GetConnectedNodes().Num() == 1)
		{
			PossibleNodes.Add(Node);
		}
	}
	if (PossibleNodes.Num() > 1) {
		ABoatNode* Start = PossibleNodes[FMath::RandRange(0, PossibleNodes.Num() - 1)];
		return Start;
	}
	else
	{
		return nullptr;
	}
	
}

TArray<FVector> UBoatNavigation::CreatePaths(ABoatNode* StartNode)
{
	if (!StartNode) return TArray<FVector>();

	TArray<ABoatNode*> Visited {};
	TArray<ABoatNode*> Queue {StartNode};
	TArray<FVector> Path {StartNode->GetActorLocation()};

	while(!Queue.IsEmpty())
	{
		ABoatNode* CurrentNode = Queue.Top();

		Visited.Add(CurrentNode);

		for (ABoatNode* ConnectedNodes : CurrentNode->GetConnectedNodes())
		{
			if (!Visited.Contains(ConnectedNodes))
			{
				Queue.Add(ConnectedNodes);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Coulnt Find Valid Path!!"))
	return TArray<FVector>();
}

TArray<FVector> UBoatNavigation::ReconstructPaths(const TMap<ABoatNode*, ABoatNode*>& CameFromMap, ABoatNode* EndNode)
{
	TArray<FVector> NodeLocations;

	const ABoatNode* NextBoatNode = EndNode;
	while (NextBoatNode)
	{
		NodeLocations.Push(NextBoatNode->GetActorLocation());
		NextBoatNode = CameFromMap[NextBoatNode];
	}
	return NodeLocations;
}

bool UBoatNavigation::HasNoNodes()
{
	return Nodes.IsEmpty();
}
