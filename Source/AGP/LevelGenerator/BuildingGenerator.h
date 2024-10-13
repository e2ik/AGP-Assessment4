// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelGenerator.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "SpawningNode.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "BuildingGenerator.generated.h"

class ALanternGenerator;
class AGardenGenerator;

UCLASS()
class AGP_API ABuildingGenerator : public ALevelGenerator
{
	GENERATED_BODY()

	friend class ATerrainGenerator;
	
public:	
	// Sets default values for this actor's properties
	ABuildingGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Variables to control the generation
	UPROPERTY(EditAnywhere)
	bool bShouldGenerateBuilding = false;

	UPROPERTY(EditAnywhere)
	int Width = 3; //How many walls wide should the building be?

	UPROPERTY(EditAnywhere)
	int Depth = 3; //How many walls deep?

	UPROPERTY(EditAnywhere)
	int Height = 3; //How many levels should there be?

	UPROPERTY(EditAnywhere)
	int ExtraRoofs = 3; //How many roofs should finish off the building?

	UPROPERTY(EditAnywhere)
	int RoofIndex; //Which roof should we use?

	UPROPERTY(EditAnywhere)
	float RoofOffset = 0; //Is there any offset for the roof to be attached to the walls?

	UPROPERTY(EditAnywhere)
	float FloorOffset = 0; //Is there any offset above the roof for the walls to attach to the floor?

	UPROPERTY()
	FVector LanternOffset = FVector(0.0, 0.0, 0.0);
	
	//Meshes
	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Wall;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Door;

	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Roofs;

	UPROPERTY(EditAnywhere)
	ALanternGenerator* LanternGenerator;

	UPROPERTY(VisibleAnywhere)
	TArray<ASpawningNode*> LanternNodes;

	

	void BuildBuilding();
	void SetBuildingGeneratorParameters(int W, int D, int H, int XRoofs, int RIndex, float ROffset, float FOffset);
	void SpawnLanternNodes(const FVector& Location, const FVector& Dimensions, bool bRotate);
	void SpawnLanterns();
	void DeleteNodes();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnBuildingByPreset(int Preset, FVector Location, FVector Dimensions, AGardenGenerator* GardenGenerator);
};
