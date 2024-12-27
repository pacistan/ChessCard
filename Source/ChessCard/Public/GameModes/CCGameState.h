#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "GameModes/FSM/CCFSM.h"
#include "CCGameState.generated.h"

class ACCPlayerState;
class ACCPlayerController;
class ACCGridManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentTimeOfPlanniningPhaseChange, float, CurrentTimeOfPlanniningPhase);

UCLASS()
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	friend class ACCGameMode;
	
	UPROPERTY(Replicated)
	TObjectPtr<ACCGridManager> GridManager;

	UPROPERTY(Replicated)
	EGameState CurrentState;

	/** List of all player controllers in the game, only True in Server */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ACCPlayerController*> PlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentTimeOfPlanniningPhase)
	float CurrentTimeOfPlanniningPhase = 60.f;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnCurrentTimeOfPlanniningPhaseChange OnCurrentTimeOfPlanniningPhaseChange;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool AddPlayerController(ACCPlayerController* PlayerController);

private:
	UFUNCTION()
	void OnRep_CurrentTimeOfPlanniningPhase();
	
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
};
