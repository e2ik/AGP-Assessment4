// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
};
