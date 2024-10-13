// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelGenerator.h"
#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "LanternGenerator.generated.h"

class USplineMeshComponent;
/**
 * 
 */
UCLASS()
class AGP_API ALanternGenerator : public ALevelGenerator
{
	GENERATED_BODY()

	friend class ATerrainGenerator;
	friend class ABuildingGenerator;

public:

	ALanternGenerator();
	void SpawnLanterns(FVector Start, FVector End);
	void SetLanternMesh(UStaticMesh* Mesh);
	void DeleteSplines();

protected:

	UPROPERTY()
	USplineComponent* Spline;

	UPROPERTY()
	TArray<USplineComponent*> SpawnedSplines;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Lantern;

	UPROPERTY(EditAnywhere)
	float DistanceBetweenLanterns = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* SplineMaterial;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* SplineMesh;
	
};
