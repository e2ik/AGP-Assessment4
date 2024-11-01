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
	const ANavigationNode* StartNode;
	const ANavigationNode* EndNode;
	bool bIsTraversable;

	FSpan()
	{
		StartNode = nullptr;
		EndNode = nullptr;
		bIsTraversable = false;
	}
	
	FSpan(const ANavigationNode* startNode, const ANavigationNode* endNode, bool isTraversable)
	{
		StartNode = startNode;
		EndNode = endNode;
		bIsTraversable = isTraversable;
		// you know, it might be better to have the span perform the sweep itself
	}
	// Returns the StartNode's location
	FVector GetStartLocation()
	{
		//UE_LOG(LogTemp, Log, TEXT("StartNode is %hs"), StartNode ? "not null" : "null")
		return StartNode ? StartNode->GetActorLocation() : FVector::ZeroVector;
	}
	// Returns the EndNode's location
	FVector GetEndLocation()
	{
		//UE_LOG(LogTemp, Log, TEXT("EndNode is %hs"), StartNode ? "not null" : "null")
		return EndNode ? EndNode->GetActorLocation() : FVector::ZeroVector;
	}
	// Returns the sum of the node locations
	FVector GetVectorSum()
	{
		return GetStartLocation() + GetEndLocation();
	}
	// Returns the midpoint of the two nodes
	FVector GetMidPoint()
	{
		return GetVectorSum() / 2;
	}
	FVector GetModifiedMidPoint()
	{
		FVector MidPoint = GetMidPoint();
		if (MidPoint.Z < 138.0f)
		{
			MidPoint.Z = 138.0f;
		}
		return MidPoint;
		
	}
	// Returns the distance between the two nodes
	float GetSpanDist()
	{
		return FVector::Distance(GetStartLocation(), GetEndLocation());
	}
	// Returns the direction vector from StartNode to EndNode
	FVector GetDirectionVector()
	{
		return GetStartLocation() - GetEndLocation();
	}
	// Returns the quaternion rotation of the span
	FQuat GetOrientation()
	{
		return GetDirectionVector().ToOrientationQuat();
	}
	// Returns the quaternion rotation of a span, modified to be level with a flat surface
	FQuat GetOrientationModified()
	{
		FRotator Rot = GetOrientation().Rotator();
		Rot.Pitch = 0;
		Rot.Roll = 0;
		return Rot.Quaternion();
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
	
	// SpanArray 
	TArray<FSpan> SpanArray;

	bool IsTraversable(ANavigationNode* StartNode, ANavigationNode* EndNode);
	
	// Check if a span exists
	bool SpanExists(ANavigationNode* StartNode, ANavigationNode* EndNode);
	
	// populates the span map; called after the nodes have been populated through SetNodesArray()
	void PopulateSpanMap();

	// functions as a pure check of the span map to see if a span exists, and is recorded as traversible. if it does not exist (moving node?) then it will perform SweepSpan();
	bool CheckSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	void AddSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	FSpan* GetSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	// the actual sweeping functionality of the old IsSpanTraversible(). 
	bool SweepSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	void DrawSpan(bool bUnblockedSpan, FVector StartLocation, FVector EndLocation);

	void TestSweep(ANavigationNode* StartNode, ANavigationNode* EndNode);
	
};
