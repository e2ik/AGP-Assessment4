// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UCLASS()
class AGP_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor*> SpawnedMeshes;

	UPROPERTY(EditAnywhere)
	USceneComponent* LocationComponent;
	
	void SpawnMesh(FString MeshName, UStaticMesh* Mesh, FVector Location, FRotator Rotation, FVector Scale);

	FVector GetMeshDimensions(UStaticMesh* Mesh);

	void DeleteMeshes();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
};
