// Fill out your copyright notice in the Description page of Project Settings.


#include "LanternGenerator.h"

#include "NavigationSystemTypes.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ALanternGenerator::ALanternGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	RootComponent = Spline;
}

void ALanternGenerator::SpawnLanterns(FVector Start, FVector End)
{
	SetActorLocation(Start);
	FVector Direction = (End - Start).GetSafeNormal();
	FVector MidPoint = (Start + End) * 0.5f;
	float BowHeight = 100.0f;
	MidPoint.Z -= BowHeight;
	Spline->ClearSplinePoints();
	Spline->AddSplinePoint(Start, ESplineCoordinateSpace::World);
	Spline->AddSplinePoint(MidPoint, ESplineCoordinateSpace::World);
	Spline->AddSplinePoint(End, ESplineCoordinateSpace::World);
	float TangentLength = FVector::Dist(Start, End) * 0.5f;
	Spline->SetTangentAtSplinePoint(0, Direction, ESplineCoordinateSpace::Local);
	Spline->SetTangentAtSplinePoint(1, Direction * TangentLength, ESplineCoordinateSpace::Local);
	Spline->SetTangentAtSplinePoint(2, Direction, ESplineCoordinateSpace::Local);
	Spline->SetMaterial(0, SplineMaterial);
	Spline->UpdateSpline();

	Spline->SetMaterial(0, SplineMaterial);
	
	float SplineLength = Spline->GetSplineLength();
	for (float D = 100.0; D < SplineLength - 100.0; D += DistanceBetweenLanterns)
	{
		FVector Location = Spline->GetLocationAtDistanceAlongSpline(D, ESplineCoordinateSpace::World);
		SpawnMesh(Lantern, Location, FRotator(0.0), FVector(1.0));
	}
	
	SpawnedSplines.Add(Spline);
	Spline = NewObject<USplineComponent>();
}

void ALanternGenerator::SetLanternMesh(UStaticMesh* Mesh)
{
	Lantern = Mesh;
}

void ALanternGenerator::DeleteSplines()
{
	SpawnedSplines.Empty();
}
