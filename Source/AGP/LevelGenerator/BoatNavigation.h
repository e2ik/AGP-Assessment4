// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoatNode.h"
#include "Subsystems/WorldSubsystem.h"
#include "BoatNavigation.generated.h"

UCLASS()
class AGP_API UBoatNavigation : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<ABoatNode*> Nodes;

public:
	void PopulateNodes(TArray<int> Map, int Height, int Width, FVector Location, FVector Dimensions); 
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	ABoatNode* GetNewBoatPath();
	bool HasNoNodes();
	
private: 
	TArray<FVector> CreatePaths(ABoatNode* StartNode);
	TArray<FVector> ReconstructPaths(const TMap<ABoatNode*, ABoatNode*>& CameFromMap, ABoatNode* EndNode);
};
