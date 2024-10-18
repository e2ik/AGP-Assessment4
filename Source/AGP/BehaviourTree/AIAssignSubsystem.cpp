#include "AIAssignSubsystem.h"
#include "AGP/Characters/EnemyCharacter.h"
#include "AGP/Characters/PlayerCharacter.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "TimerManager.h"


void UAIAssignSubsystem::OnWorldBeginPlay(UWorld& inWorld)
{
    GetEnemies();
    AssignAI();
}

void UAIAssignSubsystem::AssignAI()
{    
    for (AEnemyCharacter* Enemy : Enemies) {

        EEnemyType EnemyType = Enemy->GetEnemyType();

        switch (EnemyType) {
            case EEnemyType::STATIONARY:
                UE_LOG(LogTemp, Log, TEXT("Stationary enemy"));
                CheckBTComponent(Enemy);
                if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
                    UClass* BTClass = GameInstance->GetBTClass();
                    if (BTClass) {
                        Enemy->BTComponent = NewObject<UBTComponent>(Enemy, BTClass);
                        Enemy->BTComponent->RegisterComponent();
                        Enemy->BTComponent->AttachFlank();
                    }
                }
                break;

            case EEnemyType::ROAMING:
                UE_LOG(LogTemp, Log, TEXT("Roaming enemy"));

                CheckBTComponent(Enemy);
                if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
                    UClass* BTClass = GameInstance->GetBTClass();
                    if (BTClass) {
                        Enemy->BTComponent = NewObject<UBTComponent>(Enemy, BTClass);
                        Enemy->BTComponent->RegisterComponent();
                        Enemy->BTComponent->AttachRoaming();
                    }
                }
                break;

            case EEnemyType::PATHING:
                UE_LOG(LogTemp, Log, TEXT("Pathing enemy"));
                CheckBTComponent(Enemy);
                if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
                    UClass* BTClass = GameInstance->GetBTClass();
                    if (BTClass) {
                        Enemy->BTComponent = NewObject<UBTComponent>(Enemy, BTClass);
                        Enemy->BTComponent->RegisterComponent();
                        Enemy->BTComponent->AttachStatic();
                    }
                }

            default:
                break;
        }
    }
}

void UAIAssignSubsystem::GetEnemies()
{
    Enemies.Empty();
    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It) {
        AEnemyCharacter* Enemy = *It;
        if (Enemy) {
            Enemies.Add(Enemy);
            FVector Location = Enemy->GetActorLocation();
            FRotator Rotation = Enemy->GetActorRotation();            
            EnemySpawnInfos.Add(FEnemySpawnInfo(Location, Rotation));
        }
    }
}

void UAIAssignSubsystem::CheckBTComponent(AEnemyCharacter* Enemy)
{
    if (Enemy->BTComponent) {
        Enemy->BTComponent->UnregisterComponent();
        Enemy->BTComponent->UnInitialize();
        Enemy->BTComponent = nullptr;
    }
}

void UAIAssignSubsystem::NotifyPlayerSensed(bool Sensed, AEnemyCharacter* Enemy)
{
    EEnemyType EnemyType = Enemy->GetEnemyType();

    if (EnemyType == EEnemyType::PATHING) {
        bCanSeePlayer = Sensed;
        if (Sensed) {
            Enemy->bIsRepeatPath = false;
            // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attaching Roaming BT"));
            Enemy->BTComponent->AttachRoaming();
        } else {
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Enemy]() {
                this->WaitBeforeSwap(Enemy);
            }, 3.0f, false);
        }
    }

    if (EnemyType == EEnemyType::STATIONARY) {
        bCanSeePlayer = Sensed;
    }
}

void UAIAssignSubsystem::NotifyPlayerIsClose(bool Close, AEnemyCharacter* Enemy)
{
    if (Close) {
        bPlayerIsClose = true;
        Enemy->BTComponent->AttachRoaming();
    }
}

void UAIAssignSubsystem::WaitBeforeSwap(AEnemyCharacter* Enemy)
{
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attaching Static BT"));
    if (!bCanSeePlayer && Enemy->BTComponent) {
        Enemy->BTComponent->AttachStatic();
        Enemy->ClearPatrolPath();
        Enemy->bIsRepeatPath = true;
    }
}

void UAIAssignSubsystem::NotifyDeath(AEnemyCharacter* Enemy)
{
    if (Enemy->BTComponent) {
        //delete component
        Enemy->BTComponent->DestroyComponent();
        Enemy->BTComponent->UnInitialize();
        Enemy->BTComponent = nullptr;
    }
    Enemies.Remove(Enemy);
    Enemy->PlayDeathAnimation();

    if (Enemies.Num() == 0) {
        RespawnEnemies();
    }
}

void UAIAssignSubsystem::RespawnEnemies()
{
    UWorld* World = GetWorld();
    if (World) {
        for (const FEnemySpawnInfo& SpawnInfo : EnemySpawnInfos) {
            FActorSpawnParameters SpawnParams;
            if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
                UClass* EnemyClass = GameInstance->GetEnemyClass();
                if (EnemyClass) {
                    AEnemyCharacter* NewEnemy = World->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnInfo.Location, SpawnInfo.Rotation, SpawnParams);                    
                    if (NewEnemy) {                      
                        Enemies.Add(NewEnemy);
                    }
                }
            }
        }
    }
    AssignAI();
}
