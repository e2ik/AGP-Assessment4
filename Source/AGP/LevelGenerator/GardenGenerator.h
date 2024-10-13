// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelGenerator.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "GardenGenerator.generated.h"

class ABuildingGenerator;
class ASpawningNode;
class ATerrainGenerator;

UCLASS()
class AGP_API AGardenGenerator : public ALevelGenerator
{
	GENERATED_BODY()

	friend ATerrainGenerator;
	friend ABuildingGenerator;
	
public:	
	// Sets default values for this actor's properties
	AGardenGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bShouldRegenerate = false;

	UPROPERTY(EditAnywhere)
	bool bShouldSpawnFences = false;

	UPROPERTY(EditAnywhere)
	bool bShouldGenerateLShapedGarden;

	UPROPERTY(EditAnywhere)
	bool bShouldDelete = false;

	UPROPERTY(EditAnywhere)
	int Width = 3;

	UPROPERTY(EditAnywhere)
	int Height = 3;

	UPROPERTY(EditAnywhere)
	float Dimensions = 500;

	UPROPERTY(EditAnywhere)
	int OmitQuadrant = 1;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Trees;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Grass;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Fences;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> FencePosts;
	
	UPROPERTY(VisibleAnywhere)
	ASpawningNode* FenceStart;

	UPROPERTY(VisibleAnywhere)
	ASpawningNode* FenceEnd;
	
	void SpawnGarden();
	void SpawnLGarden(int Quadrant);
	void SpawnFences(FVector Start, FVector End, bool AddEntry, int FenceType);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetGardenParams(int NewHeight, int NewWidth);
};
