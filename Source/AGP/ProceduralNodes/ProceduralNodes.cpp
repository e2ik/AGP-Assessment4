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
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GeneratedNodes reached"));
    PossibleNodeLocations.Empty();
    MapNodes.Empty();
    FindAllGroundMeshes();
    AddNodesToLocations();
    ConnectNodes();
    OnNodesGenerated.Broadcast();
}

void UProceduralNodes::FindAllGroundMeshes()
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<AStaticMeshActor> ActorItr(World); ActorItr; ++ActorItr) {
        AStaticMeshActor* Actor = *ActorItr;

        if (Actor) {
            UStaticMeshComponent* MeshComponent = Actor->GetStaticMeshComponent();
            if (MeshComponent) {
                if (Actor->GetActorLabel().Contains("Ground_Big")) { NodeGenGroundBig(Actor, MeshComponent); }
                if (Actor->GetActorLabel().Contains("Ground_Narrow")) { NodeGenGroundNarrow(Actor, MeshComponent); }
                if (Actor->GetActorLabel().Contains("Bridge")) { NodeGenBridge(Actor, MeshComponent); }
                if (Actor->GetActorLabel().Contains("Roof")) { NodeGenBuilding(Actor, MeshComponent); }
                if (Actor->GetActorLabel().Contains("Door")) { NodeGenDoor(Actor, MeshComponent); }
                if (Actor->GetActorLabel().Contains("Gazebo")) { NodeGenGazebo(Actor, MeshComponent); }
            }
        }        
    }
}

void UProceduralNodes::GetMeshBounds(const UMeshComponent* MeshComponent, FVector& OutMinBounds, FVector& OutMaxBounds, FVector& OutMeshCenter)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    if (!MeshComponent) return;
    FBoxSphereBounds Bounds = MeshComponent->Bounds;
    OutMeshCenter = Bounds.Origin;
    OutMeshCenter.Z += Bounds.BoxExtent.Z;
    OutMinBounds = Bounds.Origin - Bounds.BoxExtent;
    OutMaxBounds = Bounds.Origin + Bounds.BoxExtent;
}

void UProceduralNodes::NodeGenGroundBig(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);

    int32 NodeCount = 0;

    for (float X = MinBounds.X; X <= MaxBounds.X; X += NodeSpacing) {
        for (float Y = MinBounds.Y; Y <= MaxBounds.Y; Y += NodeSpacing) {
            if (NodeCount >= MaxNodes) break;
            FVector Location = FVector(X, Y, MaxBounds.Z);
            FVector Start = Location + FVector(0.0f, 0.0f, 10.0f);
            FVector End = Location + FVector(0.0f, 0.0f, 1000.0f);
            if (CheckMesh(Actor, Start, End)) continue;
            Location = Location + FVector(0.0f, 0.0f, 100.0f);
            PossibleNodeLocations.Add(Location);
            NodeCount++;
        }
        if (NodeCount >= MaxNodes) break;
    }
}

void UProceduralNodes::NodeGenGroundNarrow(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);

    int32 NodeCount = 0;

    float Length = MaxBounds.X - MinBounds.X;
    float Width = MaxBounds.Y - MinBounds.Y;
    bool bIsLengthLonger = Length > Width;

    float CenterNarrowSide = bIsLengthLonger ? (MinBounds.Y + MaxBounds.Y) / 2.0f : (MinBounds.X + MaxBounds.X) / 2.0f;

    // Narrower side of the mesh as center
    for (float Coord = bIsLengthLonger ? MinBounds.X : MinBounds.Y;
        Coord <= (bIsLengthLonger ? MaxBounds.X : MaxBounds.Y);
        Coord += NodeSpacing) {
            
        if (NodeCount >= MaxNodes) break;
        FVector Location = bIsLengthLonger ? FVector(Coord, CenterNarrowSide, MaxBounds.Z) : FVector(CenterNarrowSide, Coord, MaxBounds.Z);
        FVector Start = Location + FVector(0.0f, 0.0f, 10.0f);
        FVector End = Location + FVector(0.0f, 0.0f, 1000.0f);
        if (CheckMesh(Actor, Start, End)) continue;
        Location = Location + FVector(0.0f, 0.0f, 100.0f);
        PossibleNodeLocations.Add(Location);
        NodeCount++;
    }
}

void UProceduralNodes::NodeGenBridge(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);

    float Length = MaxBounds.X - MinBounds.X;
    float Width = MaxBounds.Y - MinBounds.Y;
    bool bIsLengthLonger = Length > Width;

    MeshCenter = MeshCenter - FVector(0.0f, 0.0f, 200.0f);
    FVector Side1, Side2;

    if (bIsLengthLonger) {
        Side1 = FVector(MinBounds.X, MeshCenter.Y, MeshCenter.Z);
        Side2 = FVector(MaxBounds.X, MeshCenter.Y, MeshCenter.Z);
    } else {
        Side1 = FVector(MeshCenter.X, MinBounds.Y, MeshCenter.Z);
        Side2 = FVector(MeshCenter.X, MaxBounds.Y, MeshCenter.Z);
    }

    Side1.Z -= 150.0f;
    Side2.Z -= 150.0f;

    PossibleNodeLocations.Add(Side1);
    PossibleNodeLocations.Add(Side2);
    PossibleNodeLocations.Add(MeshCenter);

    FVector Offset;
    if (bIsLengthLonger) { Offset = FVector(0.0f, 150.0f, 0.0f); }
    else { Offset = FVector(150.0f, 0.0f, 0.0f); }

    FVector Side1Left = Side1 - Offset;
    FVector Side1Right = Side1 + Offset;
    FVector Side2Left = Side2 - Offset;
    FVector Side2Right = Side2 + Offset;

    Side1Left.Z -= 100.0f;
    Side1Right.Z -= 100.0f;
    Side2Left.Z -= 100.0f;
    Side2Right.Z -= 100.0f;

    PossibleNodeLocations.Add(Side1Left);
    PossibleNodeLocations.Add(Side1Right);
    PossibleNodeLocations.Add(Side2Left);
    PossibleNodeLocations.Add(Side2Right);
}


void UProceduralNodes::NodeGenBuilding(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);
    MeshCenter.Z = MinBounds.Z;
    MeshCenter.Z -= 250.0f;
    PossibleNodeLocations.Add(MeshCenter);

    float RoofHeightOffset = 250.0f;
    float NodeZ = MinBounds.Z - RoofHeightOffset;

    FVector Corner1 = FVector(MinBounds.X, MinBounds.Y, NodeZ);
    FVector Corner2 = FVector(MaxBounds.X, MinBounds.Y, NodeZ);
    FVector Corner3 = FVector(MaxBounds.X, MaxBounds.Y, NodeZ);
    FVector Corner4 = FVector(MinBounds.X, MaxBounds.Y, NodeZ);

    PossibleNodeLocations.Add(Corner1);
    PossibleNodeLocations.Add(Corner2);
    PossibleNodeLocations.Add(Corner3);
    PossibleNodeLocations.Add(Corner4);

    FVector MidPoint1 = (Corner1 + Corner2) / 2.0f;
    FVector MidPoint2 = (Corner2 + Corner3) / 2.0f;
    FVector MidPoint3 = (Corner3 + Corner4) / 2.0f;
    FVector MidPoint4 = (Corner4 + Corner1) / 2.0f;

    PossibleNodeLocations.Add(MidPoint1);
    PossibleNodeLocations.Add(MidPoint2);
    PossibleNodeLocations.Add(MidPoint3);
    PossibleNodeLocations.Add(MidPoint4);
}

void UProceduralNodes::NodeGenDoor(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);

    MeshCenter.Z = MinBounds.Z;
    MeshCenter.Z += 50.0f;
    FVector RightVector = MeshComponent->GetRightVector();
    float OffsetDistance = 150.0f;

    FVector RightLocation = MeshCenter + RightVector * OffsetDistance;
    FVector LeftLocation = MeshCenter - RightVector * OffsetDistance;

    PossibleNodeLocations.Add(MeshCenter);
    PossibleNodeLocations.Add(RightLocation);
    PossibleNodeLocations.Add(LeftLocation);
}



void UProceduralNodes::NodeGenGazebo(AStaticMeshActor* Actor, UStaticMeshComponent* MeshComponent)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FVector MinBounds, MaxBounds, MeshCenter;
    GetMeshBounds(MeshComponent, MinBounds, MaxBounds, MeshCenter);
    MeshCenter.Z = MinBounds.Z;
    MeshCenter.Z += 150.0f;
    PossibleNodeLocations.Add(MeshCenter);

    float Spacing = 300.0f;
    FVector NorthNode = MeshCenter + FVector(Spacing, Spacing, 0.0f); // North
    FVector SouthNode = MeshCenter + FVector(-Spacing, Spacing, 0.0f); // South
    FVector EastNode = MeshCenter + FVector(Spacing, -Spacing, 0.0f); // East
    FVector WestNode = MeshCenter + FVector(-Spacing, -Spacing, 0.0f); // West

    PossibleNodeLocations.Add(NorthNode);
    PossibleNodeLocations.Add(SouthNode);
    PossibleNodeLocations.Add(EastNode);
    PossibleNodeLocations.Add(WestNode);
}



bool UProceduralNodes::CheckMesh(AStaticMeshActor* MeshActor, FVector Start, FVector End)
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

void UProceduralNodes::AddNodesToLocations()
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;

    for (FVector Location : PossibleNodeLocations) {
        if (Location.Z > 0.0f && Location.Z < 500.0f) {
            ANavigationNode* Node = GetWorld()->SpawnActor<ANavigationNode>(Location, FRotator::ZeroRotator);
            if (Node) MapNodes.Add(Node);
        }
    }
}

void UProceduralNodes::ConnectNodes()
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (ANavigationNode* Node : MapNodes) {
        for (ANavigationNode* OtherNode : MapNodes) {
            if (Node == OtherNode) continue;

            float Distance = FVector::Dist(Node->GetActorLocation(), OtherNode->GetActorLocation());

            if (Distance < NodeSpacing * 2.5f) {
                FHitResult HitResult;
                FCollisionQueryParams TraceParams;
                TraceParams.AddIgnoredActor(Node);
                TraceParams.AddIgnoredActor(OtherNode);

                const float OffsetZ = 200.0f;
                // Perform the line trace with an upward offset
                FVector StartLocation = Node->GetActorLocation() + FVector(0.0f, 0.0f, OffsetZ);
                FVector EndLocation = OtherNode->GetActorLocation() + FVector(0.0f, 0.0f, OffsetZ);

                bool bHit = World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
                
                if (!bHit) {
                    Node->AddConnectedNode(OtherNode);
                }
            }
        }
    }
}

