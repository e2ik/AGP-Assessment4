// Fill out your copyright notice in the Description page of Project Settings.


#include "NodeSpan.h"

// Sets default values
ANodeSpan::ANodeSpan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ANodeSpan::ANodeSpan(ANavigationNode* startNode, ANavigationNode* endNode, bool isTraversable)
{
	StartNode = startNode;
	EndNode = endNode;
	bIsTraversable = isTraversable;
	// you know, it might be better to have the span perform the sweep itself
}

ANavigationNode* ANodeSpan::GetStartNode()
{
	return StartNode;
}

ANavigationNode* ANodeSpan::GetEndNode()
{
	return EndNode;
}

bool ANodeSpan::IsTraversable()
{
	return bIsTraversable;
}

FVector ANodeSpan::GetStartLocation()
{
	//UE_LOG(LogTemp, Log, TEXT("StartNode is %hs"), StartNode ? "not null" : "null")
	return StartNode ? StartNode->GetActorLocation() : FVector::ZeroVector;
}

FVector ANodeSpan::GetEndLocation()
{
	//UE_LOG(LogTemp, Log, TEXT("EndNode is %hs"), StartNode ? "not null" : "null")
	return EndNode ? EndNode->GetActorLocation() : FVector::ZeroVector;
}

FVector ANodeSpan::GetVectorSum()
{
	return GetStartLocation() + GetEndLocation();
}

FVector ANodeSpan::GetMidPoint()
{
	return GetVectorSum() / 2;
}

float ANodeSpan::GetSpanDist()
{
	return FVector::Distance(GetStartLocation(), GetEndLocation());
}

FVector ANodeSpan::GetDirectionVector()
{
	return GetStartLocation() - GetEndLocation();
}

FQuat ANodeSpan::GetOrientation()
{
	return GetDirectionVector().ToOrientationQuat();
}

// Called when the game starts or when spawned
void ANodeSpan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANodeSpan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

