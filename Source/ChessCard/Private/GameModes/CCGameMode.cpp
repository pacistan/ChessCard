#include "GameModes/CCGameMode.h"

#include "EngineUtils.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerStart.h"
#include "Player/CCPlayerState.h"

ACCGameMode::ACCGameMode(const FObjectInitializer& ObjectInitializer)
{
	GameStateClass = ACCGameState::StaticClass();
	PlayerControllerClass = ACCPlayerController::StaticClass();
	PlayerStateClass = ACCPlayerState::StaticClass();
}

void ACCGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<ACCPlayerStart> It(World); It; ++It) {
		if (ACCPlayerStart* PlayerStart = *It) {
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

void ACCGameMode::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld()) {
		for (AActor* Actor : InLevel->Actors) {
			if (ACCPlayerStart* PlayerStart = Cast<ACCPlayerStart>(Actor)) {
				ensure(!CachedPlayerStarts.Contains(PlayerStart));
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	}
}

void ACCGameMode::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (ACCPlayerStart* PlayerStart = Cast<ACCPlayerStart>(SpawnedActor)){
		CachedPlayerStarts.Add(PlayerStart);
	}
}

void ACCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	OnUserPostLogin.Broadcast(NewPlayer);
}

void ACCGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	OnUserLogout.Broadcast(Exiting);
}

AActor* ACCGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (Player) {
		TArray<ACCPlayerStart*> StarterPoints;
		for (auto StartIt = CachedPlayerStarts.CreateIterator(); StartIt; ++StartIt) {
			if (ACCPlayerStart* Start = (*StartIt).Get()) {
				StarterPoints.Add(Start);
			} else {
				StartIt.RemoveCurrent();
			}
		}

		if (APlayerState* PlayerState = Player->GetPlayerState<APlayerState>()) {
			if (PlayerState->IsOnlyASpectator()) {
				if (!StarterPoints.IsEmpty()) {
					return StarterPoints[FMath::RandRange(0, StarterPoints.Num() - 1)];
				}

				return Super::ChoosePlayerStart_Implementation(Player);;
			}
		}
		
		ACCPlayerStart* PlayerStart = nullptr;
		for (ACCPlayerStart* CCStart : StarterPoints) {
			if (CCStart->IsClaimed()) {
				continue;
			}

			PlayerStart = CCStart;
			break;
		}
		
		if (PlayerStart) {
			PlayerStart->TryClaim(Player);
		}

		return PlayerStart;
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ACCGameMode::StartPlaySequence()
{
	FSM = NewObject<UCCFSM>(this);
	FSM->OnBeginPlay();
}

TArray<ACCPlayerPawn*> ACCGameMode::GetPlayerPawns()
{
	TArray<ACCPlayerPawn*> Players;
	for(auto PlayerState :  GetGameState<AGameStateBase>()->PlayerArray)
	{
		ACCPlayerPawn* Player = Cast<ACCPlayerPawn>(PlayerState->GetPawn());
		if(Player)
		{
			Players.Add(Player);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Found %i Players"), Players.Num());
	return Players;
}
