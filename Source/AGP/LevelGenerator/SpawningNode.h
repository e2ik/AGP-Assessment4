// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawningNode.generated.h"

UCLASS()
class AGP_API ASpawningNode : public AActor
{
	GENERATED_BODY()
	friend class ALevelGenerator;

	
public:	
	// Sets default values for this actor's properties
	ASpawningNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USceneComponent* LocationComponent;

	UPROPERTY(EditAnywhere)
	TArray<ASpawningNode*> ConnectedNodes;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	void ConnectNodes(ASpawningNode* Node);
	TArray<ASpawningNode*> GetConnectedNodes();

};
