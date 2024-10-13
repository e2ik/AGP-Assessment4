#include "ProceduralNodes.h"
#include "Engine/Engine.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"



UProceduralNodes::UProceduralNodes()
{

}

void UProceduralNodes::GenerateNodes()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GeneratedNodes reached"));

    // TODO: Implement this function
    FindAllGroundMeshes();
    OnNodesGenerated.Broadcast();
}

void UProceduralNodes::FindAllGroundMeshes()
{
    UWorld* World = GetWorld();
    if (!World) return; // oopsies

    for (TActorIterator<AStaticMeshActor> ActorItr(World); ActorItr; ++ActorItr) {
        AStaticMeshActor* Actor = *ActorItr;

        // TODO: Split this, different locations for different mesh types
        // TODO: narrow pieces should just have a node in the center of the 'narrower' side
        // TODO: Square pieces ray cast up do not add locations that's under another mesh
        // TODO: Buildings raycast down from centre, if there's ground piece under, then add node in center
        // TODO: Door mesh racast down, if there's ground piece under, then add node in center
        
        if (Actor && Actor->GetActorLabel().Contains("Ground")) {
            UStaticMeshComponent* MeshComponent = Actor->GetStaticMeshComponent();
            if (MeshComponent) {
                FBoxSphereBounds Bounds = MeshComponent->Bounds;
                FVector MeshCenter = Bounds.Origin;
                MeshCenter.Z += Bounds.BoxExtent.Z;
                // PossibleNodeLocations.Add(MeshCenter);

                FVector MinBounds = Bounds.Origin - Bounds.BoxExtent;
                FVector MaxBounds = Bounds.Origin + Bounds.BoxExtent - FVector(BoundsPadding, BoundsPadding, 0.0f);

                int32 NodeCount = 0;

                for (float X = MinBounds.X + BoundsPadding; X < MaxBounds.X; X += NodeSpacing) {
                    for (float Y = MinBounds.Y + BoundsPadding; Y < MaxBounds.Y; Y += NodeSpacing) {
                        if (NodeCount >= MaxNodes) break;
                        FVector Location = FVector(X, Y, MaxBounds.Z);
                        FVector Start = Location;
                        FVector End = Location + FVector(0.0f, 0.0f, 1000.0f);
                        if (CheckMeshAbove(Actor, Start, End)) continue;
                        PossibleNodeLocations.Add(Location);
                        DrawDebugSphere(World, Location, 50.0f, 12, FColor::Red, true, 5.0f);
                        NodeCount++;
                    }
                    if (NodeCount >= MaxNodes) break;
                }
            }
        }
    }
}

bool UProceduralNodes::CheckMeshAbove(AStaticMeshActor* MeshActor, FVector Start, FVector End)
{
    UWorld* World = GetWorld();
    if (!World) return false;

    FHitResult HitResult;
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(MeshActor);

    bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
    if (bHit) {
        return HitResult.GetComponent()->IsA<UPrimitiveComponent>();
    }
    return false;
}