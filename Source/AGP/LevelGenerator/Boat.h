// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoatNavigation.h"
#include "GameFramework/Character.h"
#include "Boat.generated.h"

UCLASS()
class AGP_API ABoat : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	float BoatSpeed = 10.0f;

	UPROPERTY()
	UBoatNavigation* BoatNavigation;

	UPROPERTY()
	USceneComponent* LocationComponent;

	UPROPERTY(VisibleAnywhere)
	ABoatNode* TargetBoatNode = nullptr;

	UPROPERTY(VisibleAnywhere)
	ABoatNode* CurrentBoatNode = nullptr;

	UPROPERTY(VisibleAnywhere)
	ABoatNode* LastBoatNode = nullptr;

	void MoveAlongPath();

	UPROPERTY(EditAnywhere)
	float PathfindingError = 300.0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// virtual bool ShouldTickIfViewportsOnly() const override;

	void SetCurrentBoatTarget(ABoatNode* NewTarget);
	void GetNextBoatTarget();

};
