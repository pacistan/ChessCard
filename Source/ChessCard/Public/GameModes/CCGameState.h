#pragma once

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentTimeOfPlanniningPhaseChange, float, CurrentTimeOfPlanniningPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameStateChange, EGameState, CurrentState);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated, ReplicatedUsing = OnRep_CurrentTimeOfPlanniningPhase)
	float CurrentTimeOfPlanniningPhase = 60.f;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnCurrentTimeOfPlanniningPhaseChange OnCurrentTimeOfPlanniningPhaseChange;

	UPROPERTY(BlueprintAssignable)
	FGameStateChange OnGameStateChange;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACCPieceBase> PieceClass;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool AddPlayerController(ACCPlayerController* PlayerController);

private:
	UFUNCTION()
	void OnRep_CurrentTimeOfPlanniningPhase();

	UFUNCTION()
	void OnRep_CurrentState();
	
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
private:
	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ACCGridManager* GetGridManager()const {return GridManager;}

	UFUNCTION()
	int GetIndexOfPlayerController(ACCPlayerController* PlayerController) const {return PlayerControllers.Find(PlayerController);}

	UFUNCTION()
	ACCPlayerState* GetPlayerStateOfTeam(ETeam Team) const;

	UFUNCTION()
	void SetCurrentTimeOfPlanniningPhase(float InCurrentTimeOfPlanniningPhase);

	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EGameState InCurrentState);


	UFUNCTION(BlueprintCallable)
	EGameState GetCurrentState() const {return CurrentState;}
};
