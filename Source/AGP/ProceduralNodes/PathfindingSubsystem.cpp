// Fill out your copyright notice in the Description page of Project Settings.


#include "PathfindingSubsystem.h"

#include "BlueprintActionDatabase.h"
#include "CollisionDebugDrawingPublic.h"
#include "EngineUtils.h"
#include "NavigationNode.h"
#include "ProceduralNodes.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"

void UPathfindingSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	// Find the ProceduralNodes subsystem.
	ProceduralNodes = InWorld.GetSubsystem<UProceduralNodes>();
	if (ProceduralNodes) {
		ProceduralNodes->OnNodesGenerated.AddDynamic(this, &UPathfindingSubsystem::SetNodesArray);
	}
	// PopulateNodes();


}

TArray<FVector> UPathfindingSubsystem::GetWaypointPositions() const
{
	TArray<FVector> NodePositions;
	for (ANavigationNode* Node : Nodes)
	{
		if (Node)
		{
			NodePositions.Add(Node->GetActorLocation());
		}
	}
	return NodePositions;
}

TArray<FVector> UPathfindingSubsystem::GetRandomPath(const FVector& StartLocation)
{
	return GetPath(FindNearestNode(StartLocation), GetRandomNode());
}

TArray<FVector> UPathfindingSubsystem::GetPath(const FVector& StartLocation, const FVector& TargetLocation)
{
	return GetPath(FindNearestNode(StartLocation), FindNearestNode(TargetLocation));
}

TArray<FVector> UPathfindingSubsystem::GetPathAway(const FVector& StartLocation, const FVector& TargetLocation)
{
	return GetPath(FindNearestNode(StartLocation), FindFurthestNode(TargetLocation));
}

void UPathfindingSubsystem::PlaceProceduralNodes(const TArray<FVector>& LandscapeVertexData, int32 MapWidth, int32 MapHeight)
{
	// Need to destroy all of the current nodes in the world.
	RemoveAllNodes();
	
	// Then create and place all the nodes and store them in the ProcedurallyPlacedNodes array.
	for (int Y = 0; Y < MapHeight; Y++)
	{
		for (int X = 0; X < MapWidth; X++)
		{
			// Spawn the node in
			if (ANavigationNode* Node = GetWorld()->SpawnActor<ANavigationNode>())
			{
				Node->SetActorLocation(LandscapeVertexData[Y * MapWidth + X]);
				ProcedurallyPlacedNodes.Add(Node);
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to spawn a node for some reason. This is bad!"))
			}
			
		}
	}
	// Then add connections between all adjacent nodes.
	for (int Y = 0; Y < MapHeight; Y++)
	{
		for (int X = 0; X < MapWidth; X++)
		{
			if (ANavigationNode* CurrentNode = ProcedurallyPlacedNodes[Y * MapWidth + X]) // Make sure it's a valid ptr.
			{
				// ADD CONNECTIONS:
				// Add Left
				if (X != MapWidth-1)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[Y * MapWidth + X+1]);
				// Add Up
				if (Y != MapHeight-1)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y+1) * MapWidth + X]);
				// Add Right
				if (X != 0)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[Y * MapWidth + X-1]);
				// Add Down
				if (Y != 0)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y-1) * MapWidth + X]);
				// Add UpLeft
				if (X != MapWidth-1 && Y != MapHeight-1)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y+1) * MapWidth + X+1]);
				// Add UpRight
				if (X != 0 && Y != MapHeight-1)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y+1) * MapWidth + X-1]);
				// Add DownRight
				if (X != 0 && Y != 0)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y-1) * MapWidth+ X-1]);
				// Add DownLeft
				if (X != MapWidth-1 && Y != 0)
					CurrentNode->ConnectedNodes.Add(ProcedurallyPlacedNodes[(Y-1) * MapWidth + X+1]);
			}
		}
	}
	
}

void UPathfindingSubsystem::PopulateNodes()
{
	Nodes.Empty();

	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		Nodes.Add(*It);
		//UE_LOG(LogTemp, Warning, TEXT("NODE: %s"), *(*It)->GetActorLocation().ToString())
	}
	
}

void UPathfindingSubsystem::RemoveAllNodes()
{
	Nodes.Empty();
	ProcedurallyPlacedNodes.Empty();

	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		GetWorld()->DestroyActor(*It);
	}
}

ANavigationNode* UPathfindingSubsystem::GetRandomNode()
{
	// Failure condition
	if (Nodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The nodes array is empty."))
		return nullptr;
	}
	const int32 RandIndex = FMath::RandRange(0, Nodes.Num()-1);
	return Nodes[RandIndex];
}

ANavigationNode* UPathfindingSubsystem::FindNearestNode(const FVector& TargetLocation)
{
    if (Nodes.Num() == 0) { return nullptr; }

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    ANavigationNode* ClosestNode = nullptr;
    float MinDistance = UE_MAX_FLT;

    for (ANavigationNode* Node : Nodes)
    {
        const FVector NodeLocation = Node->GetActorLocation();
        const float Distance = FVector::Distance(TargetLocation, NodeLocation);

        FHitResult HitResult;
        FVector Start = TargetLocation;
        FVector End = NodeLocation;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(Node);

        bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

        if (!bHit && Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestNode = Node;
        }
    }

    return ClosestNode;
}


ANavigationNode* UPathfindingSubsystem::FindFurthestNode(const FVector& TargetLocation)
{
	// Failure condition.
	if (Nodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The nodes array is empty."))
		return nullptr;
	}

	// Using the minimum programming pattern to find the closest node.
	// What is the Big O complexity of this? Can you do it more efficiently?
	ANavigationNode* FurthestNode = nullptr;
	float MaxDistance = -1.0f;
	for (ANavigationNode* Node : Nodes)
	{
		const float Distance = FVector::Distance(TargetLocation, Node->GetActorLocation());
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			FurthestNode = Node;
		}
	}

	return FurthestNode;
}

TArray<FVector> UPathfindingSubsystem::GetPath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	if (!StartNode || !EndNode)
	{
		UE_LOG(LogTemp, Error, TEXT("Either the start or end node are nullptrs."))
		return TArray<FVector>();
	}

	// Setup the open set and add the start node.
	TArray<ANavigationNode*> OpenSet;
	OpenSet.Add(StartNode);

	// StartNode->GScore = UE_MAX_FLT;
	// StartNode->HScore = FVector::Distance(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	// Setup the maps that will hold the GScores, HScores and CameFrom
	TMap<ANavigationNode*, float> GScores, HScores;
	TMap<ANavigationNode*, ANavigationNode*> CameFrom;
	// You could pre-populate the GScores and HScores maps with all of the GScores (at infinity) and HScores here by looping over
	// all the nodes in the Nodes array. However it is more efficient to only calculate these when you need them
	// as some nodes might not be explored when finding a path.

	// Front Nodes check
	if (bIsFlanking) {
		NodesInFront(FlankLocation, FlankDirection);
		for (ANavigationNode* Node : FrontNodes) {
			if (Node) { CameFrom.Add(Node); }
		}
		bIsAddingNodes = false;
		bIsFlanking = false;
	}

	// Setup the start nodes G and H score.
	GScores.Add(StartNode, 0);
	HScores.Add(StartNode, FVector::Distance(StartNode->GetActorLocation(), EndNode->GetActorLocation()));
	CameFrom.Add(StartNode, nullptr);

	while (!OpenSet.IsEmpty())
	{
		// Find the node in the open set with the lowest FScore.
		ANavigationNode* CurrentNode = OpenSet[0]; // We know this exists because the OpenSet is not empty.
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			// We can be sure that all the nodes in the open set have already had their GScores and HScores calculated.
			if (GScores[OpenSet[i]] + HScores[OpenSet[i]] < GScores[CurrentNode] + HScores[CurrentNode])
			{
				CurrentNode = OpenSet[i];
			}
		}

		// Remove the current node from the OpenSet
		OpenSet.Remove(CurrentNode);

		if (CurrentNode == EndNode)
		{
			// Then we have found the path so reconstruct it and get the positions of each of the nodes in the path.
			// UE_LOG(LogTemp, Display, TEXT("PATH FOUND"))
			FrontNodes.Empty();
			return ReconstructPath(CameFrom, EndNode);
		}

		for (ANavigationNode* ConnectedNode : CurrentNode->ConnectedNodes)
		{
			
			if (!ConnectedNode) continue; // Failsafe if the ConnectedNode is a nullptr.
			// span check temporarily disabled. improving efficiency.
			if (CheckSpan(CurrentNode, ConnectedNode))				
			{
				const float TentativeGScore = GScores[CurrentNode] + FVector::Distance(CurrentNode->GetActorLocation(), ConnectedNode->GetActorLocation());
				// Because we didn't setup all the scores and came from at the start, we need to check if the connected node has a gscore
				// already otherwise set it. If it doesn't have a gscore then it won't have all the other things either so initialise them as well.
				if (!GScores.Contains(ConnectedNode))
				{
					GScores.Add(ConnectedNode, UE_MAX_FLT);
					HScores.Add(ConnectedNode, FVector::Distance(ConnectedNode->GetActorLocation(), EndNode->GetActorLocation()));
					CameFrom.Add(ConnectedNode, nullptr);
				}

				// Then update this nodes scores and came from if the tentative g score is lower than the current g score.
				if (TentativeGScore < GScores[ConnectedNode])
				{
					CameFrom[ConnectedNode] = CurrentNode;
					GScores[ConnectedNode] = TentativeGScore;
					// HScore is already set when adding the node to the HScores map.
					// Then add connected node to the open set if it isn't already in there.
					if (!OpenSet.Contains(ConnectedNode))
					{
						OpenSet.Add(ConnectedNode);
					}
				}
			//span check temporarily disabled
			}
			
			
		}
	}

	// If we get here, then no path has been found so return an empty array.
	return TArray<FVector>();
	
}

TArray<FVector> UPathfindingSubsystem::ReconstructPath(const TMap<ANavigationNode*, ANavigationNode*>& CameFromMap, ANavigationNode* EndNode)
{
	TArray<FVector> NodeLocations;

	const ANavigationNode* NextNode = EndNode;
	while(NextNode)
	{
		NodeLocations.Push(NextNode->GetActorLocation());
		NextNode = CameFromMap[NextNode];
	}

	return NodeLocations;
}


void UPathfindingSubsystem::SetNodesArray()
{
	Nodes = ProceduralNodes->GetMapNodes();

	PopulateSpanMap();
}

// Check the nodes in front of player
void UPathfindingSubsystem::NodesInFront(const FVector& PlayerLocation, const FVector& ForwardVector)
{   
    for (ANavigationNode* Node : Nodes) {
        if (!Node) { continue; }
        FVector NodeLocation = Node->GetActorLocation();
        FVector DirectionToNode = (NodeLocation - PlayerLocation).GetSafeNormal();
        float DistanceToNode = FVector::Dist(NodeLocation, PlayerLocation);

        float DotProduct = FVector::DotProduct(ForwardVector, DirectionToNode);
        float AngleDegrees = FMath::Acos(DotProduct) * (180.0f / PI);

        float MaxDistance = 1000.0f;
        float MaxAngle = 25.0f;

        if (DistanceToNode <= MaxDistance && AngleDegrees <= MaxAngle) {
            FrontNodes.Add(Node);
            // DrawDebugSphere(GetWorld(), NodeLocation, 50.0f, 12, FColor::Green, false, 5.0f);
        }
    }
}

TArray<FVector> UPathfindingSubsystem::GetPatrolPath(const FVector& StartLocation, int32 PatrolLength)
{
    if (PatrolPath.Num() > 0) { return PatrolPath; }
    if (PatrolLength <= 0) { return PatrolPath; }

    ANavigationNode* CurrentNode = FindNearestNode(StartLocation);
    PatrolPath.Add(CurrentNode->GetActorLocation());

    ANavigationNode* PreviousNode = nullptr;

    for (int32 i = 0; i < PatrolLength; ++i) {
        TArray<ANavigationNode*> ConnectedNodes = CurrentNode->GetConnectedNodes();

        // check if connection is bidirectional
        ConnectedNodes.RemoveAll([CurrentNode](ANavigationNode* Node) {
            return !Node->GetConnectedNodes().Contains(CurrentNode);
        });

        if (ConnectedNodes.Num() > 0) {
            int32 RandomIndex = FMath::RandRange(0, ConnectedNodes.Num() - 1);
            PreviousNode = CurrentNode;
            CurrentNode = ConnectedNodes[RandomIndex];
        } else {
            break;
        }
        PatrolPath.Add(CurrentNode->GetActorLocation());
    }
    return PatrolPath;
}

bool UPathfindingSubsystem::SpanExists(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	FSpan PotentialSpan = {StartNode, EndNode, false};
	return SpanArray.Contains(PotentialSpan);
}

void UPathfindingSubsystem::PopulateSpanMap()
{
	for (ANavigationNode* Node : Nodes)
	{
		if (!Node) continue;
		
		for (ANavigationNode* ConnectedNode : Node->GetConnectedNodes())
		{
			if (!ConnectedNode) continue;
			
			if (!SpanExists(Node, ConnectedNode))
			{
				AddSpan(Node, ConnectedNode);
			}
		}
	}
	
	// for (int i = 0; i < 40; i++)
	// {
	// 	int RandIndex = FMath::RandRange(0, SpanArray.Num() - 1);
	// 	FSpan Span = SpanArray[RandIndex];
	// 	FQuat BoxRot = Span.GetOrientation();
	// 	FVector MidPoint = Span.GetMidPoint();
	// 	if (MidPoint.Z < 138.0f)
	// 	{
	// 		MidPoint.Z = 138.0f;
	// 	}
	// 	if (BoxRot.Rotator().Pitch != 0.0f || BoxRot.Rotator().Roll != 0.0f)
	// 	{
	// 		FRotator Rot = BoxRot.Rotator();
	// 		Rot.Pitch = 0;
	// 		Rot.Roll = 0;
	// 		BoxRot = Rot.Quaternion();
	// 	}
	//
	// 	
	// 	float SpanDist = Span.GetSpanDist();
	// 	FVector BoxExtent = {SpanDist / 2, 34.0f, 88.0f};
	// 	// okay the positioning of the box - z coordinate should have the centre point be 88.0f (capsule height) away from the ground, which is at z 50.0f i.e 88.0f + 50.0f
	// 	FColor BoxColor = FColor::Green;
	// 	if (!Span.bIsTraversable)
	// 	{
	// 		BoxColor = FColor::Yellow;
	// 	}
	// 	DrawDebugBox(GetWorld(), MidPoint, BoxExtent, BoxRot, BoxColor, true, -1, 0, 4);
	// 	DrawDebugSphere(GetWorld(), Span.GetStartLocation(), 20.0f, 4, FColor::Orange, true, -1, 0, 4);
	// 	DrawDebugSphere(GetWorld(), Span.GetEndLocation(), 20.0f, 4, FColor::Purple, true, -1, 0, 4);
	// 	DrawDebugSphere(GetWorld(), MidPoint, 20.0f, 4, FColor::White, true, -1, 0, 4);
	// // 	// fantastic, it works like it should now, roughly. now we just need to adjust the rotation components.
	// }
	//
	// //UE_LOG(LogTemp, Log, TEXT("Span array is of size %i"), SpanArray.Num())
}

bool UPathfindingSubsystem::CheckSpan(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	FSpan Span = {StartNode, EndNode, false};
	if (!SpanArray.Contains(Span))
	{
		AddSpan(StartNode, EndNode);
		//return false;
	}

	return SpanArray.FindByKey(Span)->bIsTraversable;
}

void UPathfindingSubsystem::AddSpan(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	FSpan Span = {StartNode, EndNode, SweepSpan(StartNode, EndNode)};
	SpanArray.Add(Span);
}

FSpan* UPathfindingSubsystem::GetSpan(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	FSpan Span = {StartNode, EndNode, false};
	return SpanArray.FindByKey(Span);
}

bool UPathfindingSubsystem::SweepSpan(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	// Creating a test span to utilise its helper functions
	FSpan TestSpan = {StartNode, EndNode, false};
	
	FCollisionShape CubeShape = FCollisionShape::MakeBox(FVector{TestSpan.GetSpanDist() / 2, 34.0f, 88.0f});
	TArray<FHitResult> HitResults;
	FQuat CubeQuat = TestSpan.GetOrientationModified();
	
	if (GetWorld()->SweepMultiByChannel(HitResults, TestSpan.GetModifiedMidPoint(),
		TestSpan.GetModifiedMidPoint(), CubeQuat, ECC_WorldStatic, CubeShape))
	{
		
		for (FHitResult Hit : HitResults)
		{
			if (AStaticMeshActor* HitActor = Cast<AStaticMeshActor>(Hit.GetActor()))
			{
				if (HitActor->GetActorLabel(false).Contains("Bridge") || HitActor->GetActorLabel(false).Contains("Ground_Big"))
				{
					continue;
				}
				
				FBox HitBox = HitActor->GetComponentsBoundingBox();
				if (HitBox.GetSize().Z >= 176.0f)
				{
					//DrawSpan(false, TestSpan.GetStartLocation(), TestSpan.GetEndLocation());
					return false;
				}
			}
		}
	}
	//DrawSpan(true, TestSpan.GetStartLocation(), TestSpan.GetEndLocation());
	return true;
}

void UPathfindingSubsystem::DrawSpan(bool bUnblockedSpan, FVector StartLocation, FVector EndLocation)
{
	if (bUnblockedSpan)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, -1, 0, 2);
	}
	else
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1, 0, 2);
	}
	
}
