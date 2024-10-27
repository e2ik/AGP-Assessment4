// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SwordComponent.generated.h"

class UHealthComponent;

USTRUCT(BlueprintType)
struct FSwordStats
{
	GENERATED_BODY()

public:
	float BaseDamage = 40.0f;
	float SlashTime = 1.0f;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGP_API USwordComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwordComponent();

	void Slash(USceneComponent* Start, USceneComponent* End);

	void Block();
	void StopBlock();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartBlocking();
	UFUNCTION(BlueprintImplementableEvent)
	void OnStopBlocking();

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* BlockEffect;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	float TimeSinceLastSlash = UE_MAX_FLT;

	UPROPERTY(EditAnywhere)
	float SlashTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* SwordMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Sword Stats")
    FSwordStats SwordStats;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	USceneComponent* StartPoint;

	UPROPERTY(EditDefaultsOnly, Replicated)
	USceneComponent* EndPoint;

	// put a sword cooldown
	float CooldownDuration = 1.0f;
	bool bCanSlash = true;

	UPROPERTY()
	UNiagaraComponent* BlockEffectComponent;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	bool bIsBlocking = false;

	UFUNCTION()
	void OnRep_IsBlocking();
	
private:
	UFUNCTION(Server, Reliable)
	void ServerSlash(USceneComponent* Start, USceneComponent* End);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSlash(USceneComponent* Start, USceneComponent* End);

	bool SlashImplementation(USceneComponent* Start, USceneComponent* End);
	void SlashVisualImplementation(FVector Start, FVector End);
	void BlockVisualImplementation();

	// BLOCK
	void BlockImplementation();
	UFUNCTION(Server, Reliable)
	void ServerBlock();
	// STOPBLOCK
	void StopBlockImplementation();
	UFUNCTION(Server, Reliable)
	void ServerStopBlock();

	void StartBlocking();
	void StopBlocking();
};
