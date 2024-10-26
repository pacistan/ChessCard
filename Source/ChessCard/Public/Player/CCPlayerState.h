﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Macro/CCGetSetMacro.h"
#include "CCPlayerState.generated.h"

class ACCPlayerController;
class UCCPawnData;
/**
 * 
 */
UCLASS(HideDropdown)
class CHESSCARD_API ACCPlayerState : public APlayerState
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UFUNCTION(BlueprintCallable, Category = "CG|PlayerState")
	ACCPlayerController* GetCGPlayerController() const;
	
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UCCPawnData> PawnData;
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:
	
	UFUNCTION()
	void OnRep_PawnData();

	UFUNCTION(Server)
	void AskServerToSendPlayerIndexToPawn();

	UFUNCTION(NetMulticast)
	void SendPlayerIndexToClients(int32 PlayerIndex);
	
public:
	DECLARE_GETTER(PawnData, PawnData, TObjectPtr<const UCCPawnData>)
	void SetPawnData(const UCCPawnData* InPawnData);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void BeginPlay() override;
	
	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface
};
