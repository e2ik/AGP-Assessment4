 // Fill out your copyright notice in the Description page of Project Settings.
#include "AGPGameInstance.h"
#include "AGPGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AGP/Pickups/WeaponPickup.h"
#include "AGP/BehaviourTree/BTComponent.h"
#include "AGP/Characters/EnemyCharacter.h"
#include "AGP/Characters/PlayerCharacter.h"
#include "AGP/Characters/PlayerMeleeCharacter.h"


void UAGPGameInstance::Init()
{
    OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        Session = OnlineSubsystem->GetSessionInterface();
        Session->OnCreateSessionCompleteDelegates.AddUObject(this, &UAGPGameInstance::OnCreateSessionComplete);
    }
}

void UAGPGameInstance::CreateSession(FName SessionName)
{
    if (Session.IsValid())
    {
        FOnlineSessionSettings SessionSettings = FOnlineSessionSettings();
        SessionSettings.bIsLANMatch = true;
        SessionSettings.NumPublicConnections = 3;
        SessionSettings.bShouldAdvertise = true;
        bool bSuccess = Session->CreateSession(0, SessionName, SessionSettings);
        OnCreateSessionComplete(SessionName, bSuccess);
    }
}

void UAGPGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
    if(bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("Session Created: %s"), SessionName)
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Session Creation Failed: %s"), SessionName)
    }
}

void UAGPGameInstance::ServerTravel()
{
    
}

void UAGPGameInstance::ClientTravel()
{
    
}

 UClass* UAGPGameInstance::GetWeaponPickupClass() const
{
    return WeaponPickupClass.Get();
}

UClass* UAGPGameInstance::GetBTClass() const
{
    return BTClass.Get();
}

UClass* UAGPGameInstance::GetEnemyClass() const
{
    return EnemyClass.Get();
}

UClass* UAGPGameInstance::GetPlayerClass() const
{
    return PlayerClass.Get();
}

UClass* UAGPGameInstance::GetPlayerMeleeClass() const
{
    return PlayerMeleeClass.Get();
}

void UAGPGameInstance::SpawnGroundHitParticle(const FVector& SpawnLocation)
{
    if (GroundHitParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GroundHitParticle, SpawnLocation);
    }
}

void UAGPGameInstance::SpawnCharacterHitParticle(const FVector& SpawnLocation)
{
    if (CharacterHitParticle) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CharacterHitParticle, SpawnLocation);
    }
}

void UAGPGameInstance::SpawnGunEffect(const FVector& StartLocation, const FVector& EndLocation, int32 Color)
{
    if (GunEffect) {
        UNiagaraComponent* GunEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GunEffect, StartLocation);
        if (GunEffectComponent) {
            FVector HitColor(0.1f, 0.1f, 0.0f);
            FVector MissColor(0.0f, 0.1f, 0.1f);
            FVector NothingColor(0.1f, 0.1f, 0.1f);
            
            switch (Color) {
                case 1:
                    GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamColor"), HitColor);
                    break;
                case 2:
                    GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamColor"), MissColor);
                    break;
                case 3:
                    GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamColor"), NothingColor);
                    break;
                default:
                    GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamColor"), NothingColor);
                    break;
            }

            GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamStart"), StartLocation);
            GunEffectComponent->SetNiagaraVariableVec3(TEXT("BeamEnd"), EndLocation);
        }
    }
}

void UAGPGameInstance::PlayGunshotSoundAtLocation(const FVector& Location)
{
    if (GunshotSoundCue) {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), GunshotSoundCue, Location, FRotator::ZeroRotator);
    }
}

void UAGPGameInstance::SpawnBlockEffect(const FVector& SpawnLocation)
{
    if (BlockEffect) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BlockEffect, SpawnLocation);
    }
}

void UAGPGameInstance::PlayGunshotSound2D()
{
    if (GunshotSoundCue) {
        UGameplayStatics::PlaySound2D(GetWorld(), GunshotSoundCue);
    }
}

void UAGPGameInstance::PlayDeathSoundAtLocation(const FVector& Location)
{
    if (DeathSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundCue, Location, FRotator::ZeroRotator);
    }
}

 void UAGPGameInstance::PlayFailSound2D()
 {
    if (FailSoundCue)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), FailSoundCue);
    }
 }

 void UAGPGameInstance::PlayHurtSoundAtLocation(const FVector& Location)
 {
    if (HurtSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), HurtSoundCue, Location, FRotator::ZeroRotator);
    }
 }

 void UAGPGameInstance::PlayOughSound2D()
 {
    if (OughSoundCue)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), OughSoundCue);
    }
 }

 void UAGPGameInstance::SetSelectedPawnClass(TSubclassOf<APawn> PawnClass)
{
    SelectedPawnClass = PawnClass;
}


