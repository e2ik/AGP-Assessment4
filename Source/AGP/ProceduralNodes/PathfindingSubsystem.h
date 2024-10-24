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

// USTRUCT()
// struct FNodeSpan
// {
// 	GENERATED_BODY()
// 	
// public:
// 	UPROPERTY()
// 	ANavigationNode* StartNode;
// 	UPROPERTY()
// 	ANavigationNode* EndNode;
//
// 	FVector VectorSum;
// 	//int32 ID;
//
// 	FNodeSpan()
// 	{
// 		StartNode = EndNode = nullptr;
// 		VectorSum = FVector::ZeroVector;
// 	}
//
// 	FNodeSpan(ANavigationNode* startNode, ANavigationNode* endNode)
// 	{
// 		StartNode = startNode;
// 		EndNode = endNode;
// 		VectorSum = StartNode->GetActorLocation() + EndNode->GetActorLocation();
// 	}
//
// 	FString ToString() const
// 	{
// 		FString SpanString = "";
// 		SpanString += "First Node has name " + StartNode->GetName() + ", Position " + StartNode->GetActorLocation().ToString() + "\n";
// 		SpanString += "Last Node has name " + EndNode->GetName() + ", Position " + EndNode->GetActorLocation().ToString() + "\n";
// 		return SpanString;
// 	}
//
// 	friend bool operator==(const FNodeSpan& first, const FNodeSpan& second)
// 	{
// 		return (first.StartNode == second.StartNode && first.EndNode == second.EndNode) || (first.StartNode == second.EndNode && first.EndNode == second.StartNode);
// 	}
//
// 	// friend FVector GetHashType(const FNodeSpan& other)
// 	// {
// 	// 	return FVector(GetTypeHash(other.VectorSum));
// 	// }
//
// 	
// };

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

	// DEFUNCT --- NO LONGER USED
	// Using a modified MinDist pattern, finds the nearest node to the target location, from the current node, across ALL nodes
	// when a new minimum has been found, it checks if the path to the potential node is obstructed using IsSpanTraversible()
	ANavigationNode* FindNearestShortcutNode(const ANavigationNode* CurrentNode, const FVector& TargetLocation);

	// Performs a sweep from one node to another, checking for any hits along the path. Any hits have their heights checked,
	// being below the height of the enemy character being "traversable".
	bool IsSpanTraversable(const ANavigationNode* StartNode, const ANavigationNode* EndNode);

	// delete these 
	int32 TimesSpanChecked;
	int32 TimesPathfinding;

	// okay we make a map that records the traversed state of spans
	// probably like TMap<TArray<ANavigationNode*>, bool>;
	//TMap<TArray<ANavigationNode*>*, bool> SpanMap;
	// problem: TMap does not like it when a TArray is used as its key. perhaps create a span struct instead.
	TMap<FVector, bool> SpanMap;

	bool SpanExists(const ANavigationNode* StartNode, const ANavigationNode* EndNode);
	
};
