#include "AIDirector.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AGP/ProceduralNodes/NavigationNode.h"
#include "AGP/GameMode/MultiplayerGameMode.h"
#include "DrawDebugHelpers.h"


UAIDirector::UAIDirector()
{

}

void UAIDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Get the world time
    GameTime = GetWorld()->GetTimeSeconds();

    UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
    if (AIAssignSubsystem) {
        int32 NumOfCurrentEnemies = AIAssignSubsystem->GetNumOfEnemies();
        GEngine->AddOnScreenDebugMessage(0, 0.f, FColor::Green, FString::Printf(TEXT("Number of Enemies: %d"), NumOfCurrentEnemies));
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
    if (bPlaceStarts) { GetRandomNode(); }
    if (bSpawnEnemies) { SpawnEnemies(); }
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
}