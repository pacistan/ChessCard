// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCGameMode.generated.h"

enum class ECCOnlineMode : uint8;
class UCCPawnData;

/**
 * Post Login Event, Trigger When a player joins the game
 * Call at end Of initialization 
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCCCameModePlayerInitialized, AGameModeBase*, AController*)

UCLASS(HideDropdown)
class CHESSCARD_API ACCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	
	
public:
	// Delegate called on player initialization
	FOnCCCameModePlayerInitialized OnGameModePlayerInitialized;
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "CC|Pawn")
	const UCCPawnData* GetPawnDataForController(const AController* InController) const;
	
	// Restart (respawn) the specified player next frame
	// - If bForceReset is true, the controller will be reset this frame (abandoning the currently possessed pawn, if any)
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

protected:
	UFUNCTION()
	void HandlePartyAssignement();

	bool IsExperienceLoaded() const;
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:

	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual void InitGameState() override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~End of AGameModeBase interface
};
