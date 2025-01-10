#include "GameModes/CCGameState.h"
#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Engine/PlayerStartPIE.h"
#include "GameModes/CCEffectManagerComponent.h"
#include "GameModes/CCGameMode.h"
#include "Macro/CCLogMacro.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerStart.h"
#include "Player/CCPlayerState.h"

extern ENGINE_API float GAverageFPS;

ACCGameState::ACCGameState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	EffectManager = CreateDefaultSubobject<UCCEffectManagerComponent>("EffectManager");
}

void ACCGameState::BeginPlay()
{
	Super::BeginPlay();
	GridManager = Cast<ACCGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACCGridManager::StaticClass()));
	BPE_StartGame();
	
	if (HasAuthority()) {
		if (ACCGameMode* GameMode = Cast<ACCGameMode>(GetWorld()->GetAuthGameMode())) {
			MaxTimeOfPlanniningPhase = GameMode->GetTimeOfPlanniningPhase();
		}
	}
}

bool ACCGameState::AddPlayerController(ACCPlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) {
		return false;
	}
	
	return PlayerControllers.AddUnique(PlayerController) != INDEX_NONE;
}

void ACCGameState::AddPointToPlayer(APlayerState* Player)
{
	if (!IsValid(Player) || !PlayerScore.PlayerStates.Contains(Player)) {
		DEBUG_ERROR("try to update Score with a non valid Player")
		return;
	}
	
	int index = PlayerScore.PlayerStates.Find(Player);
	
	if (index != INDEX_NONE) {
		if (PlayerScore.Score.IsValidIndex(index)) {
			PlayerScore.Score[index]++;
		} else {
			DEBUG_ERROR("try to update Score with a non valid Index")
			return;
		} 
	}
	
	OnScoreUpdate.Broadcast(PlayerScore);
}

void ACCGameState::InitScoreArray()
{
	PlayerScore.PlayerStates = PlayerArray;
	PlayerScore.Score.Init(0, PlayerScore.PlayerStates.Num());
}

void ACCGameState::OnRep_CurrentTimeOfPlanniningPhase()
{
	OnCurrentTimeOfPlanniningPhaseChange.Broadcast(CurrentTimeOfPlanniningPhase, MaxTimeOfPlanniningPhase);
}

void ACCGameState::OnRep_CurrentState()
{
	OnGameStateChange.Broadcast(CurrentState);
}

void ACCGameState::OnRep_Score()
{
	OnScoreUpdate.Broadcast(PlayerScore);
}

void ACCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACCGameState, GridManager);
	DOREPLIFETIME(ACCGameState, CurrentTimeOfPlanniningPhase);
	DOREPLIFETIME(ACCGameState, MaxTimeOfPlanniningPhase);
	DOREPLIFETIME(ACCGameState, CurrentState);
	DOREPLIFETIME(ACCGameState, PlayerScore);
	
}

ACCPlayerState* ACCGameState::GetPlayerStateOfTeam(ETeam Team) 
{
	for (auto PlayerState : PlayerArray) {
		if (ACCPlayerState* CCPlayerState = Cast<ACCPlayerState>(PlayerState)) {
			if (CCPlayerState->GetTeam() == Team) {
				return CCPlayerState;
			}
		}
	}

	return nullptr;
}

void ACCGameState::SetCurrentTimeOfPlanniningPhase(float InCurrentTimeOfPlanniningPhase)
{
	CurrentTimeOfPlanniningPhase = InCurrentTimeOfPlanniningPhase;
	OnCurrentTimeOfPlanniningPhaseChange.Broadcast(CurrentTimeOfPlanniningPhase, MaxTimeOfPlanniningPhase);
}

void ACCGameState::SetCurrentState(EGameState InCurrentState)
{
	CurrentState = InCurrentState;
	OnGameStateChange.Broadcast(CurrentState);
}

void ACCGameState::BPC_RegisterAudioComponent(UAudioComponent* AudioComp)
{
	AudioComp->bAutoDestroy = false;
}