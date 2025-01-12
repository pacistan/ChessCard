﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Macro/CCGetSetMacro.h"
#include "Player/CCPlayerPawn.h"
#include "CCGameMode.generated.h"

class ACCPlayerController;
class ACCPlayerState;
class ACCPlayerStart;
class UCCBaseState;
class UCCFSM;
enum class ETeam : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLogin, AController*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCGameModePlayerInitialized, AController*, NewPLayer);

UCLASS()
class CHESSCARD_API ACCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY()
	TObjectPtr<UCCFSM> FSM;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCCBaseState> StartState;

	// Number of players needed to start the game (default 4)
	UPROPERTY(EditAnywhere)
	int NumOfPlayersNeeded = 4;
	
	UPROPERTY(EditAnywhere)
	float TimeOfPlanniningPhase = 120.f;

	// Cached player starts
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACCPlayerStart>> CachedPlayerStarts;

public:
	UPROPERTY(EditAnywhere)
	int BaseNumberOfCardsToDraw;
	
	// Map of all the Action of the players, recup at the end of the Draw State
	TMap<ACCPlayerState*, TArray<FPlayerActionData>> PlayerActions;

	UPROPERTY()
	TArray<TObjectPtr<ACCPlayerController>> PlayerControllers;

	UPROPERTY()
	TMap<ACCTile*, ETeam> BonusTileMap;

	UPROPERTY(EditAnywhere)
	float UnitSpawnActionDuration = 1;

	UPROPERTY(EditAnywhere)
	float TimerBetweenActions = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfPointsNeededForVictory;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnUserLogin OnUserPostLogin;

	UPROPERTY(BlueprintAssignable)
	FOnUserLogin OnUserLogout;

	UPROPERTY(BlueprintAssignable)
	FOnCCGameModePlayerInitialized OnPlayerInitialized;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable)
	void StartPlaySequence();

	UFUNCTION()
	void AddPlayerAction(ACCPlayerState* PlayerState, TArray<FPlayerActionData> Actions);

private:
	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	UFUNCTION()
	virtual TArray<ACCPlayerPawn*> GetPlayerPawns();

private:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	// Choose the good playerStart for the player, claim it and take the team associated with it
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	
	virtual void PreInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;
	
	/* ------------------------------------------ GETTERS / SETTERS  -------------------------------------------*/
public:
	UFUNCTION()
	UCCFSM* GetFSM(){return FSM;}

	UFUNCTION()
	TSubclassOf<UCCBaseState> GetStartState(){return StartState;}
	
	UFUNCTION(BlueprintCallable)
	int GetNumOfPlayersNeeded() const { return NumOfPlayersNeeded; }

	UFUNCTION(BlueprintCallable)
	void SetNumOfPlayersNeeded(int InNumOfPlayersNeeded) { NumOfPlayersNeeded = InNumOfPlayersNeeded; }

	UFUNCTION(BlueprintCallable)
	float GetTimeOfPlanniningPhase() const { return TimeOfPlanniningPhase; }

	TMap<ACCPlayerState*, TArray<FPlayerActionData>> GetPlayerActionsQueue() {return PlayerActions;}
	
};
