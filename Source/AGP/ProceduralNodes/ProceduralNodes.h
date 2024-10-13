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

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> PossibleNodeLocations;

	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMesh*> CurrentGroundMeshes;

	UPROPERTY(EditAnywhere)
	float NodeSpacing = 500.0f;

	UPROPERTY(EditAnywhere)
	int32 MaxNodes = 20;

	UPROPERTY(EditAnywhere)
	float BoundsPadding = 200.0f;

private:

	void FindAllGroundMeshes();
	bool CheckMeshAbove(AStaticMeshActor* MeshActor, FVector Start, FVector End);

};
