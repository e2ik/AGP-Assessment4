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

	// Performs a sweep from one node to another, checking for any hits along the path. Any hits have their heights checked,
	// being below the height of the enemy character being "traversable".
	bool IsSpanTraversable(const ANavigationNode* StartNode, const ANavigationNode* EndNode);

	// okay we make a map that records the traversed state of spans
	// probably like TMap<TArray<ANavigationNode*>, bool>;
	//TMap<TArray<ANavigationNode*>*, bool> SpanMap;
	// problem: TMap does not like it when a TArray is used as its key. perhaps create a span struct instead.
	TMap<FVector, bool> SpanMap;

	bool SpanExists(const ANavigationNode* StartNode, const ANavigationNode* EndNode);

	// alrighty redoing stuff!!
	
	// populates the span map; called after the nodes have been populated through SetNodesArray()
	void PopulateSpanMap();

	// functions as a pure check of the span map to see if a span exists, and is recorded as traversible. if it does not exist (moving node?) then it will perform SweepSpan();
	bool CheckSpan(ANavigationNode* StartNode, ANavigationNode* EndNode);

	// the actual sweeping functionality of the old IsSpanTraversible(). 
	bool SweepSpan(FVector StartLocation, FVector EndLocation);

	void DrawSpan(bool bUnblockedSpan, FVector StartLocation, FVector EndLocation);
	
};
