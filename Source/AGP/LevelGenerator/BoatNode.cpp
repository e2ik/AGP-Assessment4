// Fill out your copyright notice in the Description page of Project Settings.


#include "BoatNode.h"

// Sets default values
ABoatNode::ABoatNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LocationComponent = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(LocationComponent);
	bNetLoadOnClient = false;
}

// Called when the game starts or when spawned
void ABoatNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoatNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Super::Tick(DeltaTime);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0, 4, ConnectedNodes.Contains(this) ? FColor::Cyan : FColor::Magenta, false, -1, 0, 10.0);
	for (const ABoatNode* Node : ConnectedNodes)
	{
		if (Node) {
			if (Node->ConnectedNodes.Contains(this))
			{
				DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(), FColor::Purple, false, -1, 0, 5.0f);
			} else
			{
				DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(), FColor::Yellow, false, -1, 0, 5.0f);
			}
		}
	}
}

bool ABoatNode::ShouldTickIfViewportsOnly() const
{
	return true;
}

TArray<ABoatNode*> ABoatNode::GetConnectedNodes()
{
	return ConnectedNodes;
}

void ABoatNode::ConnectNodes(ABoatNode* Node)
{
	if (!Node) return;
	if (!ConnectedNodes.Contains(Node))
	{
		ConnectedNodes.Add(Node);
	}
	if (!Node->ConnectedNodes.Contains(this))
	{
		Node->ConnectedNodes.Add(this);
	}
}

ABoatNode* ABoatNode::InsertMidpointBoatNode(ABoatNode* Node)
{
	ConnectedNodes.Remove(Node);
	Node->RemoveNode(this);
	ABoatNode* NewNode = GetWorld()->SpawnActor<ABoatNode>();
	NewNode->SetActorLocation(FMath::Lerp(GetActorLocation(), Node->GetActorLocation(), 0.5f));
	ConnectNodes(NewNode);
	Node->ConnectNodes(NewNode);
	return NewNode;
}

void ABoatNode::RemoveNode(ABoatNode* Node)
{
	ConnectedNodes.Remove(Node);
}

