// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameMode.generated.h"

class UCCBaseState;
class UCCFSM;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserPostLogin, APlayerController*, NewPlayer);

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

public:
	UPROPERTY(BlueprintAssignable)
	FOnUserPostLogin OnUserPostLogin;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	UFUNCTION()
	virtual TArray<ACCPlayerPawn*> GetPlayerPawns();

	UFUNCTION()
	UCCFSM* GetFSM(){return FSM;}

	UFUNCTION()
	TSubclassOf<UCCBaseState> GetStartState(){return StartState;}
	
	UFUNCTION(BlueprintCallable)
	void StartPlaySequence();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
