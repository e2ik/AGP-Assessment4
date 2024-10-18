#include "BTComponent.h"
#include "AGP/Characters//EnemyCharacter.h"

UBTComponent::UBTComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBTComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Walking Pathing BT
void UBTComponent::AttachStatic()
{
	RootBT = NewObject<UCBTree>();
	BTName = "Walk Path BT";
	RootBT->Initialize(BTName);

	UCSequence* RootSequence = NewObject<UCSequence>();
	RootSequence->Initialize("Walk Path Root");

	UCSequence* Sequence1 = NewObject<UCSequence>();
	Sequence1->Initialize("Walk Path Base Sequence");
	UCLeaf* GeneratePathLeaf = NewObject<UCLeaf>();
	UGeneratePath* GeneratePath = NewObject<UGeneratePath>();
	GeneratePath->Initialize("GeneratePath");
	GeneratePath->InitializeType(EPathType::STATIC);
	GeneratePath->SetOwner(GetOwner());
	GeneratePathLeaf->Initialize("GeneratePathLeaf", GeneratePath);
	UCLeaf* FollowPathLeaf = NewObject<UCLeaf>();
	UFollowPath* FollowPath = NewObject<UFollowPath>();
	FollowPath->Initialize("FollowPath");
	FollowPath->SetOwner(GetOwner());
	FollowPathLeaf->Initialize("FollowPathLeaf", FollowPath);

	Sequence1->AddChild(GeneratePathLeaf);
	Sequence1->AddChild(FollowPathLeaf);

	RootBT->AddChild(Sequence1);
	bIsInitiated = true;
}

// Flanking BT
void UBTComponent::AttachFlank()
{
	RootBT = NewObject<UCBTree>();
	BTName = "Flanking BT";
	RootBT->Initialize(BTName);

	UCSequence* RootSequence = NewObject<UCSequence>();
	RootSequence->Initialize("Flank Root");

	UCSequence* Sequence1 = NewObject<UCSequence>();
	Sequence1->Initialize("Flank Base Sequence");
	UCLeaf* GeneratePathLeaf = NewObject<UCLeaf>();
	UGeneratePath* GeneratePath = NewObject<UGeneratePath>();
	GeneratePath->Initialize("GeneratePath");
	GeneratePath->InitializeType(EPathType::FLANK);
	GeneratePath->SetOwner(GetOwner());
	GeneratePathLeaf->Initialize("GeneratePathLeaf", GeneratePath);
	UCLeaf* FollowPathLeaf = NewObject<UCLeaf>();
	UFollowPath* FollowPath = NewObject<UFollowPath>();
	FollowPath->Initialize("FollowPath");
	FollowPath->SetOwner(GetOwner());
	FollowPathLeaf->Initialize("FollowPathLeaf", FollowPath);
	UCLeaf* CheckFlankPathLeaf = NewObject<UCLeaf>();
	UConditionCheck* CheckFlankPath = NewObject<UConditionCheck>();
	CheckFlankPath->Initialize("CheckFlankPath");
	CheckFlankPath->SetOwner(GetOwner());

	Sequence1->AddChild(GeneratePathLeaf);
	Sequence1->AddChild(FollowPathLeaf);

	RootBT->AddChild(Sequence1);
	bIsInitiated = true;
}

// Roaming BT: Converted FSM from Labs
void UBTComponent::AttachRoaming()
{
	RootBT = NewObject<UCBTree>();
	BTName = "Roaming BT";
	RootBT->Initialize(BTName);

	// Enemy Behaviour Sequence
	UCSequence* RootSequence = NewObject<UCSequence>();
	RootSequence->Initialize("Roaming Root");

	// Sequence 1: Can see player?
	UCSelector* Sequence1 = NewObject<UCSelector>();
	Sequence1->Initialize("Roaming Base Sequence");
	UCDecorator* CanSeePlayer = NewObject<UCDecorator>();
	CanSeePlayer->Initialize("PlayerCheck");
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetOwner());
	if (Enemy) {
		CanSeePlayer->InitializeCondition([Enemy]() {
			return Enemy->CanSeePlayer();
			});
	}
	UCLeaf* SensePlayerLeaf = NewObject<UCLeaf>();
	USense* SensePlayer = NewObject<USense>();
	SensePlayer->Initialize("SensePlayer");
	SensePlayer->SetOwner(GetOwner());
	SensePlayer->InitializeType(ESenseObject::PLAYER);
	SensePlayerLeaf->Initialize("SensePlayerLeaf", SensePlayer);
	CanSeePlayer->AddChild(SensePlayerLeaf);

	// Sequence 1: Sub seq - Cannot see player start patrolling
	UCSequence* StartPatrolling = NewObject<UCSequence>();
	StartPatrolling->Initialize("StartPatrolling");
	UCLeaf* GeneratePathLeaf = NewObject<UCLeaf>();
	UGeneratePath* GeneratePath = NewObject<UGeneratePath>();
	GeneratePath->Initialize("GeneratePath");
	GeneratePath->InitializeType(EPathType::RANDOM);
	GeneratePath->SetOwner(GetOwner());
	GeneratePathLeaf->Initialize("GeneratePathLeaf", GeneratePath);
	UCLeaf* FollowPathLeaf = NewObject<UCLeaf>();
	UFollowPath* FollowPath = NewObject<UFollowPath>();
	FollowPath->Initialize("FollowPath");
	FollowPath->SetOwner(GetOwner());
	FollowPathLeaf->Initialize("FollowPathLeaf", FollowPath);
	StartPatrolling->AddChild(GeneratePathLeaf);
	StartPatrolling->AddChild(FollowPathLeaf);

	// Sequence 2: Try and get Weapon
	UCSequence* Sequence2 = NewObject<UCSequence>();
	Sequence2->Initialize("Try and get Weapon");
	UCSelector* WeaponCheckSelector = NewObject<UCSelector>();
	WeaponCheckSelector->Initialize("WeaponCheckSelector");
	UCLeaf* WeaponCheckRedundantLeaf = NewObject<UCLeaf>();
	UConditionCheck* WeaponCheckRedundant = NewObject<UConditionCheck>();
	WeaponCheckRedundant->Initialize("WeaponCheckRedundant");
	WeaponCheckRedundant->SetOwner(GetOwner());
	WeaponCheckRedundant->InitializeCondition([Enemy]() {
		if (Enemy->HasWeapon()) { return true; }
		else { return false;}
	});
	WeaponCheckRedundantLeaf->Initialize("WeaponCheckRedundantLeaf", WeaponCheckRedundant);

	// Sequence 2: Sub seq - Path away from player until weapon is seen
	UCSequence* WeapSelectorSeq = NewObject<UCSequence>();
	WeapSelectorSeq->Initialize("WeapSelectorSeq");
	UCLeaf* PathAwayLeaf = NewObject<UCLeaf>();
	UGeneratePathAway* PathAway = NewObject<UGeneratePathAway>();
	PathAway->Initialize("PathAway");
	PathAway->SetOwner(GetOwner());
	PathAwayLeaf->Initialize("PathAwayLeaf", PathAway);
	UCLeaf* FollowPathLeaf2 = NewObject<UCLeaf>();
	UFollowPath* FollowPath2 = NewObject<UFollowPath>();
	FollowPath2->Initialize("FollowPath2");
	FollowPath2->SetOwner(GetOwner());
	FollowPathLeaf2->Initialize("FollowPathLeaf2", FollowPath2);
	UCLeaf* WeaponSightUpdateLeaf = NewObject<UCLeaf>();
	UConditionCheck* WeaponSightUpdate = NewObject<UConditionCheck>();
	WeaponSightUpdate->Initialize("WeaponSightUpdate");
	WeaponSightUpdate->SetOwner(GetOwner());
	WeaponSightUpdate->InitializeCondition([Enemy]() {
		return Enemy->CanSeeWeapon();
	});
	WeaponSightUpdateLeaf->Initialize("WeaponSightUpdateLeaf", WeaponSightUpdate);


	// Sequence 3: Has seen the weapon so move towards it.
	UCSequence* Sequence3 = NewObject<UCSequence>();
	Sequence3->Initialize("Move to Weapon");
	UCLeaf* GeneratePathToWeaponLeaf = NewObject<UCLeaf>();
	UGeneratePath* GeneratePathToWeapon = NewObject<UGeneratePath>();
	GeneratePathToWeapon->Initialize("GeneratePathToWeapon");
	GeneratePathToWeapon->InitializeType(EPathType::WEAPON);
	GeneratePathToWeapon->SetOwner(GetOwner());
	GeneratePathToWeaponLeaf->Initialize("GeneratePathToWeaponLeaf", GeneratePathToWeapon);
	UCLeaf* FollowPathToWeaponLeaf = NewObject<UCLeaf>();
	UFollowPath* FollowPathToWeapon = NewObject<UFollowPath>();
	FollowPathToWeapon->Initialize("FollowPathToWeapon");
	FollowPathToWeapon->SetOwner(GetOwner());
	FollowPathToWeaponLeaf->Initialize("FollowPathToWeaponLeaf", FollowPathToWeapon);
	UCLeaf* HasWeaponUpdateLeaf = NewObject<UCLeaf>();
	UConditionCheck* HasWeaponUpdate = NewObject<UConditionCheck>();
	HasWeaponUpdate->Initialize("HasWeaponUpdate");
	HasWeaponUpdate->SetOwner(GetOwner());
	HasWeaponUpdate->InitializeCondition([Enemy]() {
		return Enemy->HasWeapon();
	});
	HasWeaponUpdateLeaf->Initialize("HasWeaponUpdateLeaf", HasWeaponUpdate);

	// Sequence 4: Now Has Weapon, Seen player so should engage
	UCSequence* Sequence4 = NewObject<UCSequence>();
	Sequence4->Initialize("Engage Player");
	UCSelector* HealthCheck = NewObject<UCSelector>();
	HealthCheck->Initialize("HealthCheck");
	UCLeaf* HealthCheckLeaf = NewObject<UCLeaf>();
	UConditionCheck* HealthCheckAction = NewObject<UConditionCheck>();
	UCSequence* PathToPlayerSequence = NewObject<UCSequence>();
	PathToPlayerSequence->Initialize("PathToPlayerSequence");
	HealthCheckAction->Initialize("HealthCheckAction");
	HealthCheckAction->SetOwner(GetOwner());
	HealthCheckAction->InitializeCondition([Enemy]() {
		return Enemy->IsHealthBelowThreshold();
	});
	HealthCheckLeaf->Initialize("HealthCheckLeaf", HealthCheckAction);
	UCLeaf* GeneratePathToPlayerLeaf = NewObject<UCLeaf>();
	UGeneratePath* GeneratePathToPlayer = NewObject<UGeneratePath>();
	GeneratePathToPlayer->Initialize("GeneratePathToPlayer");
	GeneratePathToPlayer->InitializeType(EPathType::PLAYER);
	GeneratePathToPlayer->SetOwner(GetOwner());
	GeneratePathToPlayerLeaf->Initialize("GeneratePathToPlayerLeaf", GeneratePathToPlayer);
	UCLeaf* FollowPathToPlayerLeaf = NewObject<UCLeaf>();
	UFollowPath* FollowPathToPlayer = NewObject<UFollowPath>();
	FollowPathToPlayer->Initialize("FollowPathToPlayer");
	FollowPathToPlayer->SetOwner(GetOwner());
	FollowPathToPlayerLeaf->Initialize("FollowPathToPlayerLeaf", FollowPathToPlayer);


	// Parallel: if Health is good, follow path to player and fire at player
	UCParallel* RunAndShoot = NewObject<UCParallel>();
	RunAndShoot->Initialize("RunAndShoot");
	UCLeaf* FireAtPlayerLeaf = NewObject<UCLeaf>();
	UFireWeapon* FireAtPlayer = NewObject<UFireWeapon>();
	FireAtPlayer->Initialize("FireAtPlayer");
	FireAtPlayer->SetOwner(GetOwner());
	FireAtPlayerLeaf->Initialize("FireAtPlayerLeaf", FireAtPlayer);

	UCSelector* DeathSelector = NewObject<UCSelector>();
	DeathSelector->Initialize("DeathSelector");
	UCLeaf* DeadthCheckLeaf = NewObject<UCLeaf>();
	UConditionCheck* DeadthCheck = NewObject<UConditionCheck>();
	DeadthCheck->Initialize("DeadthCheck");
	DeadthCheck->SetOwner(GetOwner());
	DeadthCheck->InitializeCondition([Enemy]() {
		return Enemy->IsDead();
	});
	DeadthCheckLeaf->Initialize("DeathCheckLeaf", DeadthCheck);

	UCSequence* LowHeathSeq = NewObject<UCSequence>();
	LowHeathSeq->Initialize("LowHeathSeq");
	UCLeaf* PathAwayLeaf2 = NewObject<UCLeaf>();
	UGeneratePathAway* PathAway2 = NewObject<UGeneratePathAway>();
	PathAway2->Initialize("PathAway2");
	PathAway2->SetOwner(GetOwner());
	PathAwayLeaf2->Initialize("PathAwayLeaf2", PathAway2);
	UCLeaf* FollowPathLeaf3 = NewObject<UCLeaf>();
	UFollowPath* FollowPath3 = NewObject<UFollowPath>();
	FollowPath3->Initialize("FollowPath3");
	FollowPath3->SetOwner(GetOwner());
	FollowPathLeaf3->Initialize("FollowPathLeaf3", FollowPath3);

	DeathSelector->AddChild(DeadthCheckLeaf);
	DeathSelector->AddChild(LowHeathSeq);
	LowHeathSeq->AddChild(PathAwayLeaf2);
	LowHeathSeq->AddChild(FollowPathLeaf3);

	RunAndShoot->AddChild(FollowPathToPlayerLeaf);
	RunAndShoot->AddChild(FireAtPlayerLeaf);

	// Parallel demo - Follow path to player and fire at player
	PathToPlayerSequence->AddChild(GeneratePathToPlayerLeaf);
	PathToPlayerSequence->AddChild(RunAndShoot);

	HealthCheck->AddChild(HealthCheckLeaf);
	HealthCheck->AddChild(PathToPlayerSequence);

	Sequence1->AddChild(CanSeePlayer);
	Sequence1->AddChild(StartPatrolling);

	Sequence2->AddChild(WeaponCheckSelector);
	WeaponCheckSelector->AddChild(WeaponCheckRedundantLeaf);
	WeaponCheckSelector->AddChild(WeapSelectorSeq);
	WeapSelectorSeq->AddChild(PathAwayLeaf);
	WeapSelectorSeq->AddChild(FollowPathLeaf2);
	WeapSelectorSeq->AddChild(WeaponSightUpdateLeaf);

	Sequence3->AddChild(GeneratePathToWeaponLeaf);
	Sequence3->AddChild(FollowPathToWeaponLeaf);
	Sequence3->AddChild(HasWeaponUpdateLeaf);


	Sequence4->AddChild(HealthCheck);
	Sequence4->AddChild(DeathSelector);
	
	RootSequence->AddChild(Sequence1);
	RootSequence->AddChild(Sequence2);
	RootSequence->AddChild(Sequence3);
	RootSequence->AddChild(Sequence4);

	RootBT->AddChild(RootSequence);
	bIsInitiated = true;
}

// Called every frame
void UBTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimeAccumulator += DeltaTime;

	CurrentStatus = RootBT->Process();
	if (TimeAccumulator >= TickInterval && bIsInitiated) {
		TimeAccumulator = 0.0f;
		EStatus ChildStatus = RootBT->GetChild(0)->GetNodeStatus();
		if (ChildStatus != EStatus::RUNNING) RootBT->Reset();
	}
	
}

void UBTComponent::UnInitialize()
{
	bIsInitiated = false;
}

void UBTComponent::TreeReset()
{
	RootBT->Reset();
}
