#include "MultiplayerGameMode.h"

#include "AGP/Characters/PlayerCharacter.h"
#include "AGP/Characters/PlayerMeleeCharacter.h"
#include "AGP/Characters/EnemyCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "AGP/BehaviourTree/AIAssignSubsystem.h"
#include "AGP/GameMode/AIDirector.h"
#include "AGP/GameMode/AGPGameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void AMultiplayerGameMode::BeginPlay() {
	Super::BeginPlay();
	
	// // check level
	// FString CurrentLevelName = GetWorld()->GetMapName();
	// // CurrentLevelName = FPaths::GetBaseFilename(CurrentLevelName);
	// if (CurrentLevelName.ToLower().Contains("procgenlevel")) {
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is a procedural level"));
	// }
	// if (CurrentLevelName.ToLower().Contains("titlescreen"))
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is the Title Screen"));
	// }
}

void AMultiplayerGameMode::StartPlay() {
	Super::StartPlay();
	// testing failed
}

UClass* AMultiplayerGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (UAGPGameInstance* GameInstance = Cast<UAGPGameInstance>(GetGameInstance()))
	{
		if (GameInstance->SelectedPawnClass)
		{
			return GameInstance->SelectedPawnClass;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMultiplayerGameMode::RespawnPlayer(AController* Controller)
{
    if (Controller)
    {
        // Get currently possessed pawn
        APawn* CurrentPawn = Controller->GetPawn();

        //If the game level is title screen should just spawn TitleScreenControllers with constant location, rotation, and scale
		if (GetWorld()->GetMapName().Contains("titlescreen"))
		{
			Controller->UnPossess();
				if(CurrentPawn)
				{
					CurrentPawn->Destroyed();
				}
			FVector Location = FVector(3220.0f, -6340.0f, 280.0f);
			FRotator Rotation = FRotator(0.0f, 0.0f, 159.9f);
			ATitleScreenController* TitleScreenController = GetWorld()->SpawnActor<ATitleScreenController>(TitleScreenControllerClass, Location, Rotation);
			Controller->Possess(TitleScreenController);
			return;
		}

        // Check if it's APlayerCharacter
        if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CurrentPawn))
        {
			// tickable pass (probably a bad way to do it)
			UAIDirector* Director = GetWorld()->GetSubsystem<UAIDirector>();
			if (Director) { Director->RegisterPlayerDeath(Controller); }


            Controller->UnPossess();
            PlayerCharacter->Destroy();

            // Spawn new PlayerCharacter
			FVector StartLocation = GetPlayerStartLocation();
            APlayerCharacter* NewCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, StartLocation, FRotator::ZeroRotator);
            Controller->Possess(NewCharacter);

            if (NewCharacter)
            {
				// give them weapons on spawn
				if (!NewCharacter->WeaponComponent) {
					NewCharacter->WeaponComponent = NewObject<UWeaponComponent>(NewCharacter, UWeaponComponent::StaticClass());
					NewCharacter->WeaponComponent->SetWeaponStats(FWeaponStats());
					NewCharacter->WeaponComponent->RegisterComponent();
					NewCharacter->ChooseCharacterMesh();
					// NewCharacter->DrawUI(); Not needed anymore due to PlayerCharacter.cpp changes
					NewCharacter->EquipWeapon(true);
				}

            }
        }
        // Check if it's APlayerMeleeCharacter
        if (APlayerMeleeCharacter* PlayerMeleeCharacter = Cast<APlayerMeleeCharacter>(CurrentPawn))
        {
            Controller->UnPossess();
            PlayerMeleeCharacter->Destroy();

            // Spawn new PlayerMeleeCharacter
			FVector StartLocation = GetPlayerStartLocation();
            APlayerMeleeCharacter* NewMeleeCharacter = GetWorld()->SpawnActor<APlayerMeleeCharacter>(PlayerMeleeCharacterClass, StartLocation, FRotator::ZeroRotator);
            Controller->Possess(NewMeleeCharacter);

            if (NewMeleeCharacter)
            {
				if (!NewMeleeCharacter->SwordComponent) {
					NewMeleeCharacter->SwordComponent = NewObject<USwordComponent>(NewMeleeCharacter, USwordComponent::StaticClass());
					NewMeleeCharacter->SwordComponent->RegisterComponent();
					NewMeleeCharacter->ChooseCharacterMesh();
					NewMeleeCharacter->DrawUI();
					NewMeleeCharacter->EquipSword(true);
				}
            }
        }
    }
}



void AMultiplayerGameMode::RespawnEnemy(AController* Controller)
{
	if (Controller)
	{
		if (AEnemyCharacter* CurrentlyPossessedCharacter = Cast<AEnemyCharacter>(Controller->GetPawn()))
		{
			Controller->UnPossess();
			UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
			AIAssignSubsystem->NotifyDeath(CurrentlyPossessedCharacter);
			CurrentlyPossessedCharacter->Destroy();
		}
	}

	// Grab a spawn location
	// AActor* SpawnLocationActor = nullptr;
	// for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	// {
	// 	SpawnLocationActor = *It;
	// 	break;
	// }

	// if (SpawnLocationActor)
	// {
	// 	FActorSpawnParameters SpawnParams;
	// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// 	SpawnParams.Owner = this;
	// 	SpawnParams.Instigator = nullptr;
	// 	SpawnParams.bNoFail = true;

	// 	AEnemyCharacter* NewEnemyCharacter = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyCharacterClass, SpawnLocationActor->GetActorLocation(), SpawnLocationActor->GetActorRotation(), SpawnParams);
	// 	if (NewEnemyCharacter)
	// 	{
	// 		Controller->Possess(NewEnemyCharacter);
	// 		UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
	// 		AIAssignSubsystem->AssignAI();
	// 	}
	// }
}

void AMultiplayerGameMode::SpawnPlayers()
{
    UWorld* World = GetWorld();
    if (!World) return;

	AActor* OriginalStart = nullptr;
	// Find original start and delete it
	for (TActorIterator<APlayerStart> It(World); It; ++It) {
		// Print the names of all player starts
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *FString::Printf(TEXT("Found PlayerStart: %s"), *It->GetName()));
		
		if (It->PlayerStartTag == "DeleteMe") {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Found the original start"));
			OriginalStart = *It;
			OriginalStart->Destroy();
			break;
		}
	}

    // Get all connected PlayerControllers
    TArray<AActor*> PlayerControllerActors;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerController::StaticClass(), PlayerControllerActors);

    // Iterate through all connected player controllers
    for (AActor* Actor : PlayerControllerActors)
    {
        APlayerController* PlayerController = Cast<APlayerController>(Actor);
        if (PlayerController)
        {
            // Restart the player, which will use a PlayerStart automatically
            RespawnPlayer(PlayerController);
        }
    }
}

void AMultiplayerGameMode::SpawnEnemy(const FVector& Location)
{
	UWorld* World = GetWorld();
	if (!World) return;
	FActorSpawnParameters SpawnParams;
    if (const UAGPGameInstance* GameInstance = GetWorld()->GetGameInstance<UAGPGameInstance>()) {
        UClass* EnemyClass = GameInstance->GetEnemyClass();
        if (EnemyClass) {
            AEnemyCharacter* NewEnemy = World->SpawnActor<AEnemyCharacter>(EnemyClass, Location, FRotator::ZeroRotator, SpawnParams);

			// give them weapons on spawn
			if (NewEnemy) {
				if (!NewEnemy->WeaponComponent) {
					NewEnemy->WeaponComponent = NewObject<UWeaponComponent>(NewEnemy, UWeaponComponent::StaticClass());
					NewEnemy->WeaponComponent->RegisterComponent();
					NewEnemy->MulticastEquipWeapon(true);
				}
			}

        }
    }
	UAIAssignSubsystem* AIAssignSubsystem = GetWorld()->GetSubsystem<UAIAssignSubsystem>();
	AIAssignSubsystem->GetEnemies();
	AIAssignSubsystem->AssignAI();

}

FVector AMultiplayerGameMode::GetPlayerStartLocation()
{
	AActor* SpawnLocationActor = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		SpawnLocationActor = *It;
		break;
	}
	return SpawnLocationActor->GetActorLocation();
}

