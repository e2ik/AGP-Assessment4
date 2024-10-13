// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingGenerator.h"
#include "GardenGenerator.h"
#include "LanternGenerator.h"
#include "BoatNavigation.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Util/IteratorUtil.h"
#include "TerrainGenerator.generated.h"

class AWallGenerator;

UCLASS()
class AGP_API ATerrainGenerator : public ALevelGenerator
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATerrainGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bShouldUpdate = false;

	UPROPERTY(EditAnywhere)
	bool bShouldDelete = false;
	
	UPROPERTY(EditAnywhere)
	int Height = 10;

	UPROPERTY(EditAnywhere)
	int Width = 10;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* FloorBlock;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* FloorBlock2;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* DebugMesh;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* BridgeMesh;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* RiverPosts;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Gazebo;
	
	UPROPERTY(EditAnywhere)
	ABuildingGenerator* BuildingGenerator;

	UPROPERTY(EditAnywhere)
	AGardenGenerator* GardenGenerator;

	UPROPERTY(EditAnywhere)
	ALanternGenerator* LanternGenerator;

	UPROPERTY(EditAnywhere)
	AWallGenerator* WallGenerator;

	UPROPERTY(VisibleAnywhere)
	TArray<int> Map;

	UPROPERTY(EditAnywhere)
	TArray<ASpawningNode*> LanternNodes;

	UPROPERTY(EditAnywhere)
	UBoatNavigation* BoatNavigator;
	
	void GenerateTerrain();
	void CreateMap();
	void PlaceGround(int x, int y, FVector Dimensions, FVector ActorLocation);
	void PlaceWater(int x, int y, FVector Dimensions, FVector ActorLocation);
	void SpawnLanternPost();
	void DeleteNodes();
	void SpawnLanterns();
	void DeleteLanterns();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
