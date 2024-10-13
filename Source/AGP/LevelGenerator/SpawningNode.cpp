// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawningNode.h"

// Sets default values
ASpawningNode::ASpawningNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LocationComponent = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(LocationComponent);
}

// Called when the game starts or when spawned
void ASpawningNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawningNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0, 4, ConnectedNodes.Contains(this) ? FColor::Blue : FColor::Red, false, -1, 0, 10.0);
	// for (const ASpawningNode* Node : ConnectedNodes)
	// {
	// 	if (Node->ConnectedNodes.Contains(this))
	// 	{
	// 		DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(), FColor::Green, false, -1, 0, 5.0f);
	// 	} else
	// 	{
	// 		DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(), FColor::Red, false, -1, 0, 5.0f);
	// 	}
	// }
}

bool ASpawningNode::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ASpawningNode::ConnectNodes(ASpawningNode* Node)
{
	if (ConnectedNodes.Contains(Node))
	{
		return;
	}
	if (Node->ConnectedNodes.Contains(this))
	{
		return;
	}
	ConnectedNodes.Add(Node);
}

TArray<ASpawningNode*> ASpawningNode::GetConnectedNodes()
{
	return ConnectedNodes;	
}