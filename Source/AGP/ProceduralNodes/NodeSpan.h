// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "GameFramework/Actor.h"
#include "NodeSpan.generated.h"

UCLASS()
class AGP_API ANodeSpan : public AActor
{
	GENERATED_BODY()

private:
	ANavigationNode* StartNode = nullptr;
	ANavigationNode* EndNode = nullptr;
	bool bIsTraversable = false;
	
public:	
	// Sets default values for this actor's properties
	ANodeSpan();
	
	ANodeSpan(ANavigationNode* startNode, ANavigationNode* endNode, bool isTraversable);

	// Returns a pointer to the StartNode
	ANavigationNode* GetStartNode();

	// Returns a pointer to the EndNode
	ANavigationNode* GetEndNode();

	// Returns true if the node is traversable, false otherwise
	bool IsTraversable();
	
	// Returns the StartNode's location
	FVector GetStartLocation();
	
	// Returns the EndNode's location
	FVector GetEndLocation();
	
	// Returns the sum of the node locations
	FVector GetVectorSum();
	
	// Returns the midpoint of the two nodes
	FVector GetMidPoint();
	
	// Returns the distance between the two nodes
	float GetSpanDist();
	
	// Returns the direction vector from StartNode to EndNode
	FVector GetDirectionVector();
	
	// Returns the quaternion rotation of the span
	FQuat GetOrientation();

	friend bool operator==(const ANodeSpan& SpanOne, const ANodeSpan& SpanTwo)
	{
		return SpanOne.StartNode == SpanTwo.StartNode && SpanOne.EndNode == SpanTwo.EndNode || SpanOne.StartNode == SpanTwo.EndNode && SpanOne.EndNode == SpanTwo.StartNode;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
