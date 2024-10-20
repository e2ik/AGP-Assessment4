// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMeleeCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

void APlayerMeleeCharacter::UpdateHealthBar(float HealthPercent)
{
	if(PlayerHUD && IsLocallyControlled())
	{
		PlayerHUD->SetHealthBar(HealthPercent);
	}
}

void APlayerMeleeCharacter::DrawUI()
{
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerHUD = CreateWidget<UPlayerCharacterHUD>(PlayerController, PlayerHUDClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
	UpdateHealthBar(1.0f);
}

void APlayerMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	DrawUI();
}

void APlayerMeleeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
	}
}

void APlayerMeleeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerMeleeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerMeleeCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerMeleeCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerMeleeCharacter::SlashSword);
		// Input->BindAction(DashAction, ETriggerEvent::Started, this, &ABaseMeleeCharacter::Dash);
	}
}

void APlayerMeleeCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardVector = GetActorForwardVector();
	AddMovementInput(ForwardVector, MovementVector.X);
	const FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, MovementVector.Y);
}

void APlayerMeleeCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>() * LookSensitivity;
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerMeleeCharacter::SlashSword(const FInputActionValue& Value)
{
	Slash();
}
