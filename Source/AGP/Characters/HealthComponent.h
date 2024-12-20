// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

	bool IsDead();
	float GetCurrentHealth() const;
	float GetCurrentHealthPercentage() const;
	void ApplyDamage(float DamageAmount);
	void ApplyHealing(float HealingAmount);
	void ResetHealth();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	bool bCanDamage = true;

	void SetCanDamage(bool CanDamage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	float MaxHealth = 100.0f;
	UPROPERTY(ReplicatedUsing=UpdateHealthBar)
	float CurrentHealth;
	bool bIsDead = false;

	void OnDeath();
	UFUNCTION()
	void UpdateHealthBar();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDeathSound();
	UFUNCTION(Server, Reliable)
	void ServerPlayDeathSound();

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
