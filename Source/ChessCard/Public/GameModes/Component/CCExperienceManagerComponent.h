// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCExperienceManagerComponent.generated.h"

class UCCExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCCExperienceLoaded, const UCCExperienceDefinition*);

// TODO : add loading screen process
// TODO : add loading Decors process
// TODO : add loading game Process like (Deck, special rules etc...) ?

enum class ECCExperienceLoadState : uint8
{
	Unloaded,
	Loading,
	Loaded,
	Deactivating
};

/**
 *  Actor Component to put  on the GameState Only
 */
UCLASS()
class CHESSCARD_API UCCExperienceManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentExperience)
	TObjectPtr<const UCCExperienceDefinition> CurrentExperience;

	ECCExperienceLoadState LoadState = ECCExperienceLoadState::Unloaded;
	
	/** Delegate called when the experience has finished loading */
	FOnCCExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnCCExperienceLoaded OnExperienceLoaded_LowPriority;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UCCExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	
	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnCCExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnCCExperienceLoaded::FDelegate&& Delegate);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const UCCExperienceDefinition* GetCurrentExperienceChecked() const;

	// Returns true if the experience is fully loaded
	UFUNCTION(BlueprintCallable, Category = "CC|Experience")
	bool IsExperienceLoaded() const;


private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	
	/*------------------------------------------- OVERRIDES -------------------------------------------*/

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
