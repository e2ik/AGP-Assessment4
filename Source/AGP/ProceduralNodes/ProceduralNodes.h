// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NavigationNode.h"
#include "Engine/StaticMeshActor.h"
#include "ProceduralNodes.generated.h"

class AProceduralLandscape;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNodesGenerated);

UCLASS()
class AGP_API UProceduralNodes : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UProceduralNodes();
	void GenerateNodes();

	UPROPERTY(BlueprintAssignable)
	FOnNodesGenerated OnNodesGenerated;

	TArray<ANavigationNode*> GetMapNodes() const { return MapNodes; }

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> PossibleNodeLocations;

	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMesh*> CurrentGroundMeshes;

	UPROPERTY(EditAnywhere)
	float NodeSpacing = 500.0f;

	UPROPERTY(EditAnywhere)
	int32 MaxNodes = 50;

	UPROPERTY(EditAnywhere)
	float BoundsPadding = 200.0f;

	UPROPERTY(VisibleAnywhere)
	TArray<ANavigationNode*> MapNodes;

private:

	void FindAllGroundMeshes();
	void GetMeshBounds(const UMeshComponent* MeshComponent, FVector& OutMinBounds, FVector& OutMaxBounds, FVector& OutMeshCenter);
	bool CheckMesh(AStaticMeshActor* MeshActor, FVector Start, FVector End);
	void NodeGenGroundBig(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void NodeGenGroundNarrow(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void NodeGenBridge(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void NodeGenBuilding(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void NodeGenDoor(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void NodeGenGazebo(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent);
	void AddNodesToLocations();
	void ConnectNodes();

};
