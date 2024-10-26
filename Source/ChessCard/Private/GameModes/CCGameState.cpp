// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CCGameState.h"

#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerStart.h"

extern ENGINE_API float GAverageFPS;

ACCGameState::ACCGameState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	ServerFPS = 0.0f;
}



AActor* ACCGameState::ChoosePlayerStart(AController* Player)
{
	if (Player) {
#if WITH_EDITOR
		if (APlayerStart* PlayerStart = FindPlayFromHereStart(Player)) {
			return PlayerStart;
		}
#endif
		TArray<ACCPlayerStart*> StarterPoints;
		for (auto StartIt = CachedPlayerStarts.CreateIterator(); StartIt; ++StartIt) {
			if (ACCPlayerStart* Start = (*StartIt).Get()) {
				StarterPoints.Add(Start);
			} else {
				StartIt.RemoveCurrent();
			}
		}

		AActor* PlayerSpawn = GetFirstRandomUnoccupiedPlayerStart(Player, StarterPoints);

		if (ACCPlayerStart* CGPlayerSpawn = Cast<ACCPlayerStart>(PlayerSpawn)){
			CGPlayerSpawn->TryClaim(Player);
		}

		return PlayerSpawn;
	}

	return nullptr;
}

void ACCGameState::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// TODO Call back for blueprint 
}

#if WITH_EDITOR
APlayerStart* ACCGameState::FindPlayFromHereStart(AController* Player)
{
	// Only 'Play From Here' for a player controller, bots etc. should all spawn from normal spawn points.
	if (Player->IsA<APlayerController>()) {
		if (UWorld* World = GetWorld()) {
			for (TActorIterator<APlayerStart> It(World); It; ++It){
				if (APlayerStart* PlayerStart = *It) {
					if (PlayerStart->IsA<APlayerStartPIE>()) {
						// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
						return PlayerStart;
					}
				}
			}
		}
	}

	return nullptr;
}
#endif

void ACCGameState::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (ACCPlayerStart* PlayerStart = Cast<ACCPlayerStart>(SpawnedActor)) {
		CachedPlayerStarts.Add(PlayerStart);
	}
}

APlayerStart* ACCGameState::GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<ACCPlayerStart*>& FoundStartPoints) const
{
	if (Controller) {
		TArray<ACCPlayerStart*> UnOccupiedStartPoints;

		for (ACCPlayerStart* StartPoint : FoundStartPoints) {
			if (!StartPoint->IsClaimed()) {
				UnOccupiedStartPoints.Add(StartPoint);
			}
		}

		if (UnOccupiedStartPoints.Num() > 0) {
			return UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		} 
	}

	return nullptr;
}

void ACCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void ACCGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetLocalRole() == ROLE_Authority) {
		ServerFPS = GAverageFPS;
	}
}

void ACCGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(UWorld* World = GetWorld())
	{
		World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));
		for (TActorIterator<ACCPlayerStart> It(World); It; ++It) {
			if (ACCPlayerStart* PlayerStart = *It){
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	}
}

void ACCGameState::BeginPlay()
{
	Super::BeginPlay();
	GridManager = Cast<ACCGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACCTile::StaticClass()));
}
