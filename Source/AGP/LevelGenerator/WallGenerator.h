// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelGenerator.h"
#include "WallGenerator.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API AWallGenerator : public ALevelGenerator
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> Walls;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Tower;

	UPROPERTY(EditAnywhere)
	bool bShouldUpdate = false;
	
	UPROPERTY(EditAnywhere)
	int DefaultWidth = 10;

	UPROPERTY(EditAnywhere)
	int DefaultHeight = 10;

	UPROPERTY(EditAnywhere)
	float DefaultScale = 3000.0;

	UPROPERTY(EditAnywhere)
	float WallOffsetDebug = 0;
	
public:
	void GenerateWalls(int Width, int Height, float Scale, TArray<int> Map);
	virtual void Tick(float DeltaTime) override;
	void SpawnWalls(FVector StartPosition, FVector EndPosition, int Type);
	void SpawnFullWall(TArray<int> Key, float Scale, FVector StartPosition, FVector EndPosition, int RandWallSelection);

};
