// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"

#include "Engine/StaticMeshActor.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LocationComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(LocationComponent);
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALevelGenerator::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ALevelGenerator::SpawnMesh(FString MeshName, UStaticMesh* Mesh, FVector Location, FRotator Rotation, FVector Scale)
{
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh is null"));
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);
	if (MeshActor && MeshActor->GetStaticMeshComponent())
	{
        if (MeshName.Contains(TEXT("Fence"))) {
            MeshActor->GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
        }
		MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
		MeshActor->SetMobility(EComponentMobility::Movable);
		MeshActor->SetActorLocation(Location);
		MeshActor->SetActorRotation(Rotation);
		MeshActor->SetActorScale3D(Scale);
		MeshActor->SetActorLabel(MeshName);
		SpawnedMeshes.Add(MeshActor);
	}
}

FVector ALevelGenerator::GetMeshDimensions(UStaticMesh* Mesh)
{
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh is null"));
		return FVector();
	}
	FBoxSphereBounds Bounds = Mesh->GetBounds();
	FVector BoxExtent = Bounds.BoxExtent;
	FVector MeshDimensions = BoxExtent * 2;
	return MeshDimensions;
}

void ALevelGenerator::DeleteMeshes()
{
	if (!SpawnedMeshes.IsEmpty())
	{
		for(AStaticMeshActor* Mesh : SpawnedMeshes)
		{
			if (Mesh) {
				GetWorld()->DestroyActor(Mesh);
			}
		}
		SpawnedMeshes.Empty();
	}
}
