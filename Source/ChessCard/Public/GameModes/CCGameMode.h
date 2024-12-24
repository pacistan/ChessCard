// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameMode.generated.h"

class UCCBaseState;
class UCCFSM;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLogin, AController*, NewPlayer);

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

public:
	UPROPERTY(BlueprintAssignable)
	FOnUserLogin OnUserPostLogin;

	UPROPERTY(BlueprintAssignable)
	FOnUserLogin OnUserLogout;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable)
	void StartPlaySequence();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	UFUNCTION()
	virtual TArray<ACCPlayerPawn*> GetPlayerPawns();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	/* ------------------------------------------ GETTERS / SETTERS  -------------------------------------------*/

	UFUNCTION()
	UCCFSM* GetFSM(){return FSM;}

	UFUNCTION()
	TSubclassOf<UCCBaseState> GetStartState(){return StartState;}
	
	UFUNCTION(BlueprintCallable)
	int GetNumOfPlayersNeeded() const { return NumOfPlayersNeeded; }
};
