// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CCGameMode.h"

#include "GameModes/CCGameState.h"
#include "GameModes/Component/CCExperienceManagerComponent.h"
#include "Experience/CCExperienceDefinition.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPawnData.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"

ACCGameMode::ACCGameMode(const FObjectInitializer& ObjectInitializer)
{
	GameStateClass = ACCGameState::StaticClass();
	PlayerControllerClass = ACCPlayerController::StaticClass();
	PlayerStateClass = ACCPlayerState::StaticClass();
	DefaultPawnClass = ::ACCPlayerPawn::StaticClass();
	// TODO : set HUDClass
}

const UCCPawnData* ACCGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr){
		if (const ACCPlayerState* CCPS = InController->GetPlayerState<ACCPlayerState>()){
			if (const UCCPawnData* PawnData = CCPS->GetPawnData()) {
				return PawnData;
			}
		}
	}

	
	check(GameState);
	UCCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UCCExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded()) {
		const UCCExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr) {
			return Experience->DefaultPawnData;
		}
		
		DEBUG_ERROR("Experience is loaded and there's still no pawn data");
	}
	
	return nullptr;
}

void ACCGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	// Wait for the next frame to give time to initialize startup settings
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandlePartyAssignement);
}

void ACCGameMode::HandlePartyAssignement()
{
	FPrimaryAssetId ExperienceId;
	
	// see if the world settings has a default experience
	/* if (!ExperienceId.IsValid()) {
		if (ACCWorldSettings* TypedWorldSettings = Cast<ACCWorldSettings>(GetWorldSettings())) {
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		}
	}*/
	
	if (ExperienceId.IsValid()) {
		DEBUG_LOG("Identified experience %s", *ExperienceId.ToString());
		UCCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UCCExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	} else {
		DEBUG_LOG("Failed to identify experience, loading never end");
	}
}

bool ACCGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UCCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UCCExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

UClass* ACCGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UCCPawnData* PawnData = GetPawnDataForController(InController)) {
		if (PawnData->PawnClass) {
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ACCGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;
	
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer)) {
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo)) {
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		} else {
			DEBUG_ERROR("Game mode was unable to spawn Pawn of class [%s] at [%s].", *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	} else {
		DEBUG_ERROR("Game mode was unable to spawn Pawn due to NULL pawn class.");
	}

	return nullptr;
}

bool ACCGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// Don't put AnyCode Here
	return false;
}

// here Handle Spawn of the Pawn For New Player
void ACCGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded()) {
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* ACCGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (ACCGameState* CGGameState = Cast<ACCGameState>(GameState)) {
		return CGGameState->ChoosePlayerStart(Player);
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ACCGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (ACCGameState* CGGameState = Cast<ACCGameState>(GameState)) {
		return CGGameState->FinishRestartPlayer(NewPlayer, StartRotation);
	}
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

void ACCGameMode::InitGameState()
{
	Super::InitGameState();
	
	// Spawn any players that are already attached
	// TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr)) {
			if (PlayerCanRestart(PC)) {
				RestartPlayer(PC);
			}
		}
	}
}

void ACCGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void ACCGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr)) {
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller)) {
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

void ACCGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);
	
	// If we tried to spawn a pawn, and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer)) {
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer)) {
			// If it's a player don't loop forever, maybe something changed, and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC)) {
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			} else {
				UE_LOG(LogCard, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		} else {
			// For bot if there is 
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	} else {
		UE_LOG(LogCard, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}
