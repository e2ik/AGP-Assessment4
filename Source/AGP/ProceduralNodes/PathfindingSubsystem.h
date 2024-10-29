// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "Subsystems/WorldSubsystem.h"
#include "PathfindingSubsystem.generated.h"

class ANavigationNode;
class UProceduralNodes;
/**
 * 
 */

USTRUCT()
struct FSpan
{
	GENERATED_BODY()

public:
	ANavigationNode* StartNode;
	ANavigationNode* EndNode;
	bool bIsTraversable;

	FSpan()
	{
		StartNode = nullptr;
		EndNode = nullptr;
		bIsTraversable = false;
	}
	
	FSpan(ANavigationNode* startNode, ANavigationNode* endNode, bool isTraversable)
	{
		StartNode = startNode;
		EndNode = endNode;
		bIsTraversable = isTraversable;
	}
	FVector GetStartLocation()
	{
		return StartNode->GetActorLocation();
	}
	FVector GetEndLocation()
	{
		return EndNode->GetActorLocation();
	}

	friend bool operator==(const FSpan& SpanOne, const FSpan& SpanTwo)
	{
		return SpanOne.StartNode == SpanTwo.StartNode && SpanOne.EndNode == SpanTwo.EndNode || SpanOne.StartNode == SpanTwo.EndNode && SpanOne.EndNode == SpanTwo.StartNode;
	}
};

UCLASS()
class AGP_API UPathfindingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION()
	void SetNodesArray();


	TArray<FVector> GetWaypointPositions() const;
	TArray<FVector> GetRandomPath(const FVector& StartLocation);
	TArray<FVector> GetPath(const FVector& StartLocation, const FVector& TargetLocation);
	TArray<FVector> GetPathAway(const FVector& StartLocation, const FVector& TargetLocation);
	TArray<FVector> GetPatrolPath(const FVector& StartLocation, int32 PatrolLength);

	void PlaceProceduralNodes(const TArray<FVector>& LandscapeVertexData, int32 MapWidth, int32 MapHeight);
	void ClearPatrolPath() { PatrolPath.Empty(); }

	UPROPERTY()
	bool bIsFlanking = false;
	UPROPERTY()
	bool bIsAddingNodes = false;

	UPROPERTY()
	FVector FlankLocation;
	UPROPERTY()
	FVector FlankDirection;
	UFUNCTION()
	void NodesInFront(const FVector& PlayerLocation, const FVector& ForwardVector);

protected:
	
	TArray<ANavigationNode*> Nodes;

	// Procedural Map Logic
	TArray<ANavigationNode*> ProcedurallyPlacedNodes;

	UPROPERTY()
	UProceduralNodes* ProceduralNodes;

	UPROPERTY()
	TArray<FVector> PatrolPath;
	UPROPERTY()
	TArray<ANavigationNode*> FrontNodes;

private:

	void PopulateNodes();
	void RemoveAllNodes();
	ANavigationNode* GetRandomNode();
	ANavigationNode* FindNearestNode(const FVector& TargetLocation);
	ANavigationNode* FindFurthestNode(const FVector& TargetLocation);
	TArray<FVector> GetPath(ANavigationNode* StartNode, ANavigationNode* EndNode);
	static TArray<FVector> ReconstructPath(const TMap<ANavigationNode*, ANavigationNode*>& CameFromMap, ANavigationNode* EndNode);

	// --- Obstacle Detection ---
	
	// Span Map uses the CrossProduct + VectorSum of the two node actors as the key
	TMap<FVector, bool> SpanMap;

	// 
	bool SpanExists(const ANavigationNode* StartNode, const ANavigationNode* EndNode);

	// alrighty redoing stuff!!
	
	// populates the span map; called after the nodes have been populated through SetNodesArray()
	void PopulateSpanMap();

	// functions as a pure check of the span map to see if a span exists, and is recorded as traversible. if it does not exist (moving node?) then it will perform SweepSpan();
	bool CheckSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	// the actual sweeping functionality of the old IsSpanTraversible(). 
	bool SweepSpan(FVector StartLocation, FVector EndLocation);

	void DrawSpan(bool bUnblockedSpan, FVector StartLocation, FVector EndLocation);

	void TestSweep(ANavigationNode* StartNode, ANavigationNode* EndNode);
	
};
