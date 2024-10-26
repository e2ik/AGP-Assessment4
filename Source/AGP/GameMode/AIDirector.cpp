#include "AIDirector.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AGP/ProceduralNodes/NavigationNode.h"
#include "AGP/GameMode/MultiplayerGameMode.h"
#include "AGP/Characters/PlayerCharacter.h"
#include "AGP/Characters/PlayerMeleeCharacter.h"
#include "AGP/BehaviourTree/AIAssignSubsystem.h"
#include "AGP/Pickups/PickupManagerSubsystem.h"
#include "AGP/GameMode/AGPGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


UAIDirector::UAIDirector()
{

}

void UAIDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Get the world time
    GameTime = GetWorld()->GetTimeSeconds();
    if (GameTime - LastExecutionTime >= 30.0f) {
        bTimerCheck = true;
        LastExecutionTime = GameTime;
    }

    UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
    if (AIAssignSubsystem) {
        int32 NumOfCurrentEnemies = AIAssignSubsystem->GetNumOfEnemies();
        // GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT("Number of Enemies: %d"), NumOfCurrentEnemies));
    }

    // throttle tick
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if ((CurrentTime - LastTickTime) >= DesiredTickInterval) {
        UWorld* World = GetWorld();
        if (!World) return;
        if (World->GetNetMode() == NM_Client) return;
        RunCustomTick();
        LastTickTime = CurrentTime;
    }
}

void UAIDirector::RunCustomTick()
{
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Custom Tick"));
    if (!bHasFoundNodes) { FindAllNodes(); }
    if (bPlaceStarts) { GetRandomNode(); SetPlayersMap(); }
    if (bSpawnEnemies) { SpawnEnemies(); }
    if (bStartBT) { RunBT(); }

}

void UAIDirector::FindAllNodes()
{
    // clear nodes array if not empty
    if (Nodes.Num() > 0) { Nodes.Empty(); }

    // find nodes
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ANavigationNode> ActorItr(World); ActorItr; ++ActorItr) {
        ANavigationNode* Node = *ActorItr;
        if (Node) Nodes.Add(Node);
    }

    bHasFoundNodes = true;
    bPlaceStarts = true;
}

void UAIDirector::GetRandomNode()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const int32 MaxAttempts = 10;
    for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt) {
        // Get a random node
        if (Nodes.Num() > 0) {
            int32 RandomIndex = FMath::RandRange(0, Nodes.Num() - 1);
            ANavigationNode* RandomNode = Nodes[RandomIndex];
            if (RandomNode) {
                FVector NodeLocation = RandomNode->GetActorLocation();

                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(RandomNode);
                float TraceLength = 250.0f;
                FVector Directions[] = {
                    NodeLocation + FVector(TraceLength, 0.0f, 0.0f),
                    NodeLocation + FVector(-TraceLength, 0.0f, 0.0f),
                    NodeLocation + FVector(0.0f, TraceLength, 0.0f),
                    NodeLocation + FVector(0.0f, -TraceLength, 0.0f)
                };

                bool bHit = false;
                // Perform line traces in each direction
                for (const FVector& Direction : Directions) {
                    bHit = World->LineTraceSingleByChannel(
                        HitResult,
                        NodeLocation,
                        Direction,
                        ECC_Visibility,
                        CollisionParams
                    );
                    if (bHit) { break; }
                }

                if (!bHit) {
                    PlacePlayerStarts(NodeLocation);
                    GenerateEnemySpawn(RandomNode);
                    return;
                }
            }
        }
    }
}



void UAIDirector::PlacePlayerStarts(const FVector& Location)
{
    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < NumOfStarts; ++i) {
        FVector RandomOffset = FVector(FMath::RandRange(-100.f, 100.f), FMath::RandRange(-200.f, 200.f), 0.f);
        FVector SpawnLocation = Location + RandomOffset;
        APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    }

    // notify GameMode that player starts have been placed and spawn via gamemode
    AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(World->GetAuthGameMode());
    if (GameMode) { GameMode->SpawnPlayers(); }

    bPlaceStarts = false;
}

void UAIDirector::GenerateEnemySpawn(ANavigationNode* CenterNode)
{
    UWorld* World = GetWorld();
    if (!World || !CenterNode) return;
    if (SpawnRadiusLocations.Num() > 0) { SpawnRadiusLocations.Empty(); }

    const float Radius = 4000.0f;
    const float InnerRadius = 1500.0f;
    FVector CenterLocation = CenterNode->GetActorLocation();

    for (ANavigationNode* Node : Nodes) {
        if (Node && Node != CenterNode) {
            float Distance = FVector::Dist(CenterLocation, Node->GetActorLocation());
            if (Distance <= Radius && Distance >= InnerRadius) {
                SpawnRadiusLocations.Add(Node->GetActorLocation());
            }
        }
    }
    bSpawnEnemies = true;
}

void UAIDirector::SpawnEnemies()
{
    bSpawnEnemies = false;
    UWorld* World = GetWorld();
    if (!World) return;
    AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(World->GetAuthGameMode());
    if (GameMode) {
        for (int32 i = 0; i < NumOfEnemies; ++i) {
            int32 RandomIndex = FMath::RandRange(0, SpawnRadiusLocations.Num() - 1);
            FVector SpawnLocation = SpawnRadiusLocations[RandomIndex];
            GameMode->SpawnEnemy(SpawnLocation);
        }
    }
    bStartBT = true;
}

void UAIDirector::SetPlayersMap()
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (PlayerIndexMap.Num() > 0) { PlayerIndexMap.Empty(); }
    if (PlayerControllerMap.Num() > 0) { PlayerControllerMap.Empty(); }

    for (int32 i = 0; i < NumOfStarts; ++i) {
        FString PlayerName = FString::Printf(TEXT("Player%d"), i + 1);
        PlayerIndexMap.Add(i, PlayerName);
    }

    for (int32 i = 0; i < NumOfStarts; ++i) {
        PlayerControllerMap.Add(i, -1);
    }
}

void UAIDirector::RegisterPlayerDeath(AController* Controller)
{
    NumOfPlayerDeaths++;
    UWorld* World = GetWorld();
    if (!World) return;

    int32 PlayerNumber = -1;

    AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(World->GetAuthGameMode());
    if (GameMode && Controller) {
        APlayerState* PlayerState = Controller->GetPlayerState<APlayerState>();
        if (PlayerState) {
            PlayerNumber = PlayerState->GetPlayerId();
            if (!PlayerControllerMap.Contains(PlayerNumber) && !bPlaceStarts) {
                for (auto& Pair : PlayerControllerMap) {
                    if (Pair.Value == -1) {
                        Pair.Value = PlayerNumber;
                        break;
                    }
                }
            }
        }
    }

    // Now find the PlayerKey in PlayerIndexMap
    const int32* PlayerKey = PlayerControllerMap.FindKey(PlayerNumber);
    if (PlayerKey) {
        // Use PlayerKey to get the player name from PlayerIndexMap
        const FString* PlayerNamePtr = PlayerIndexMap.Find(*PlayerKey);
        if (PlayerNamePtr) {
            // Log the death message with the player name
            //TODO: call a hud update can pass this string I guess
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s has died"), **PlayerNamePtr));
        }
    }
}

void UAIDirector::RunBT()
{
    ConstructBT();
    if (!bBTConstructed) return;

    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Running BT"));
    CurrentStatus = RootBT->Process();
    EStatus ChildStatus = RootBT->GetChild(0)->GetNodeStatus();
    if (ChildStatus != EStatus::RUNNING) RootBT->Reset();
}

void UAIDirector::ConstructBT() {
    RootBT = NewObject<UCBTree>();
    BTName = "AI Director BT";
    RootBT->Initialize(BTName);

	UCSelector* RootSelector = NewObject<UCSelector>();
	RootSelector->Initialize("Main Sequence");

    UCDecorator* CheckEnemies = NewObject<UCDecorator>();
    CheckEnemies->Initialize("Check Enemies");
    CheckEnemies->InitializeCondition([this]() {
        UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
        if (AIAssignSubsystem) {
            int32 NumOfCurrentEnemies = AIAssignSubsystem->GetNumOfEnemies();
            return NumOfCurrentEnemies <= 0;
        }
        return false;
    });

    USpawn* SpawnAction = NewObject<USpawn>();
    SpawnAction->PassDirector(this);

    UCDecorator* CheckPlayerDeaths = NewObject<UCDecorator>();
    CheckPlayerDeaths->Initialize("Check Player Deaths");
    CheckPlayerDeaths->InitializeCondition([this]() {
        return NumOfPlayerDeaths >= 2;
    });

    UDecreaseCount* DecreaseAction = NewObject<UDecreaseCount>();
    DecreaseAction->PassDirector(this);

    UCDecorator* CheckTotalDeaths = NewObject<UCDecorator>();
    CheckTotalDeaths->Initialize("Check Total Deaths");
    CheckTotalDeaths->InitializeCondition([this]() {
        return TotalDeathResets >= 2;
    });
    USpawnWeapon* SpawnWeaponAction = NewObject<USpawnWeapon>();
    SpawnWeaponAction->PassDirector(this);

    UCDecorator* CheckGameTime = NewObject<UCDecorator>();
    CheckGameTime->Initialize("Check Game Time");
    CheckGameTime->InitializeCondition([this]() {
        return bTimerCheck;
    });
    USpawnWeapon* SpawnWeaponAction2 = NewObject<USpawnWeapon>();
    SpawnWeaponAction2->PassDirector(this);



    RootSelector->AddChild(CheckEnemies);
    RootSelector->AddChild(CheckPlayerDeaths);
    RootSelector->AddChild(CheckTotalDeaths);
    RootSelector->AddChild(CheckGameTime);
    CheckGameTime->AddChild(SpawnWeaponAction2);
    CheckTotalDeaths->AddChild(SpawnWeaponAction);
    CheckPlayerDeaths->AddChild(DecreaseAction);
    CheckEnemies->AddChild(SpawnAction);
    RootBT->AddChild(RootSelector);

    bBTConstructed = true;
}

void UAIDirector::IncreaseNumOfEnemies()
{
    if (NumOfEnemies < MaxEnemies) {
        NumOfEnemies++;
    }
}

void UAIDirector::DecreaseNumOfEnemies()
{
    if (NumOfEnemies > 4) {
        NumOfEnemies--;
    }
}

void UAIDirector::SpawnWeaponAtPlayer()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Get the game instance
    UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GameInstance) return;

    // Get the weapon pickup class from the game instance
    UClass* WeaponPickupClass = GameInstance->GetWeaponPickupClass();
    if (!WeaponPickupClass) return;

    int32 WeaponLevel = 1;
    if (TotalDeathResets >= 3) { WeaponLevel = 4; }
    else if (TotalDeathResets >= 2) { WeaponLevel = 3; }
    else if (TotalDeathResets >= 1) { WeaponLevel = 2; }

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, 
            FString::Printf(TEXT("Total Death Resets: %d"), TotalDeathResets));

    for (TActorIterator<APlayerCharacter> ActorItr(World); ActorItr; ++ActorItr) {
        APlayerCharacter* Player = *ActorItr;
        if (Player) {
            UPickupManagerSubsystem* PickupSubsystem = World->GetSubsystem<UPickupManagerSubsystem>();
            if (PickupSubsystem) {
                PickupSubsystem->SpawnWeaponPickupNearPlayer(Player->GetActorLocation(), WeaponLevel);
            }
        }
    }
}

