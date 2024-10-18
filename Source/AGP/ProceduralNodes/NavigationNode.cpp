#include "NavigationNode.h"
#include "EngineUtils.h"


// TODO: Check Multiplayer make sure it only runs on server
// Sets default values
ANavigationNode::ANavigationNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LocationComponent"));
	RootComponent = LocationComponent;

}

// Called when the game starts or when spawned
void ANavigationNode::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ANavigationNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug Draw Sphere
	// DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 4, FColor::Blue, false, -1, 0, 5);

	for (ANavigationNode* Node : ConnectedNodes) {
		if (Node) {
			// check reciprocal connection
			bool bIsReciprocalNode = Node->ConnectedNodes.Contains(this);
			FColor LineColor = bIsReciprocalNode ? FColor::Green : FColor::Red; 
			// DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(), LineColor, false, -1, 0, 2.0f);
		}
	}

}

bool ANavigationNode::ShouldTickIfViewportsOnly() const { return true; }

void ANavigationNode::AddConnectedNode(ANavigationNode* Node)
{
	if (Node && !ConnectedNodes.Contains(Node)) {
		ConnectedNodes.Add(Node);
	}
}

