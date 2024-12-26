#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Macro/CCGetSetMacro.h"
#include "CCPlayerState.generated.h"

enum class ETeam : uint8;
class UCCExperienceDefinition;
class ACCPlayerController;
class UCCPawnData;

UCLASS()
class CHESSCARD_API ACCPlayerState : public APlayerState
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	
	// The player's team
	UPROPERTY(VisibleAnywhere)
	ETeam Team;
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
public:
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void BeginPlay() override;

	/* ------------------------------------------ GETTERS / SETTERS ------------------------------------*/
public:
	UFUNCTION(BlueprintCallable)
	ETeam GetTeam() const { return Team; }
	
	UFUNCTION(BlueprintCallable)
	void SetTeam(ETeam InTeam) { Team = InTeam; }
};
