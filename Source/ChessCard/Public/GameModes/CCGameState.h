﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "GameModes/FSM/CCFSM.h"
#include "CCGameState.generated.h"

class UCCEffectManagerComponent;
class ACCPlayerState;
class ACCPlayerController;
class ACCGridManager;
class ACCPieceBase;

USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<APlayerState*> PlayerStates;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<int> Score;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentTimeOfPlanniningPhaseChange, float, CurrentTimeOfPlanniningPhase, float, MaxTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameStateChange, EGameState, CurrentState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdate, FPlayerScore, Score); 

UCLASS()
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	friend class ACCGameMode;
	
	UPROPERTY(Replicated)
	TObjectPtr<ACCGridManager> GridManager;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCEffectManagerComponent> EffectManager;
	
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentState)
	EGameState CurrentState;

	/** List of all player controllers in the game, only True in Server */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ACCPlayerController*> PlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_CurrentTimeOfPlanniningPhase)
	float CurrentTimeOfPlanniningPhase = 60.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float MaxTimeOfPlanniningPhase = 60.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_Score)
	FPlayerScore PlayerScore;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnCurrentTimeOfPlanniningPhaseChange OnCurrentTimeOfPlanniningPhaseChange;

	UPROPERTY(BlueprintAssignable)
	FGameStateChange OnGameStateChange;

	// Call back Ui for Score 
	UPROPERTY(BlueprintAssignable)
	FOnScoreUpdate OnScoreUpdate;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACCPieceBase> PieceClass;

	UPROPERTY()
	bool IsGameOver;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool AddPlayerController(ACCPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void AddPointToPlayer(APlayerState* Player);
	
	UFUNCTION(BlueprintCallable)
	void InitScoreArray();

private:
	UFUNCTION()
	void OnRep_CurrentTimeOfPlanniningPhase();

	UFUNCTION()
	void OnRep_CurrentState();
	
	UFUNCTION()
	void OnRep_Score();

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
private:
	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ACCGridManager* GetGridManager()const {return GridManager;}

	UFUNCTION()
	int GetIndexOfPlayerController(ACCPlayerController* PlayerController) const {return PlayerControllers.Find(PlayerController);}

	UFUNCTION(BlueprintCallable)
	ACCPlayerState* GetPlayerStateOfTeam(ETeam Team);

	
	UFUNCTION()
	void SetCurrentTimeOfPlanniningPhase(float InCurrentTimeOfPlanniningPhase);

	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EGameState InCurrentState);

	UFUNCTION(BlueprintCallable)
	EGameState GetCurrentState() const {return CurrentState;}

	UFUNCTION()
	UCCEffectManagerComponent* GetEffectManager() {return EffectManager;};

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_StartGame();

	UFUNCTION(BlueprintCallable)
	void BPC_RegisterAudioComponent(UAudioComponent* AudioComp);

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnPieceEndMove(EEffectType EffectType, bool IsDivineAnger);

	UFUNCTION()
	FPlayerScore GetScores(){return PlayerScore;}
};
