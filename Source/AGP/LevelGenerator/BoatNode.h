// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoatNode.generated.h"

UCLASS()
class AGP_API ABoatNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoatNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USceneComponent* LocationComponent;

	UPROPERTY(EditAnywhere)
	TArray<ABoatNode*> ConnectedNodes;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	void ConnectNodes(ABoatNode* Node);
	TArray<ABoatNode*> GetConnectedNodes();
};
