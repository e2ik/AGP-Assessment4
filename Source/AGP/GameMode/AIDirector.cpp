#include "AIDirector.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AGP/ProceduralNodes/NavigationNode.h"

UAIDirector::UAIDirector()
{

}

TStatId UAIDirector::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UAIDirector, STATGROUP_Tickables);
}

void UAIDirector::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bHasFoundNodes = false;
}

void UAIDirector::Deinitialize()
{
    Super::Deinitialize();
}

void UAIDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Custom Tick"));
    if (!bHasFoundNodes) { FindAllNodes(); }
    GetRandomNode();
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
}

void UAIDirector::GetRandomNode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    // get a random node
    if (Nodes.Num() > 0) {
        int32 RandomIndex = FMath::RandRange(0, Nodes.Num() - 1);
        ANavigationNode* RandomNode = Nodes[RandomIndex];
        if (RandomNode) {
            PlacePlayerStarts(RandomNode->GetActorLocation());
        }
    }
}

void UAIDirector::PlacePlayerStarts(const FVector& Location)
{
    // Place player starts
    UWorld* World = GetWorld();
    if (!World) return;

    // Spawn player starts
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Ensure you are spawning an APlayerStart
    APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(Location, FRotator::ZeroRotator, SpawnParams);
    if (PlayerStart) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Start Placed"));
    }
}
