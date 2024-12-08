
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnEndCardDraw);

class UCCHandComponent;
class UCCDeckComponent;

UCLASS(HideCategories(Rendering, Collision, Actor, Input, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick))
class CHESSCARD_API ACCPlayerPawn : public APawn
{
	GENERATED_BODY()

	ACCPlayerPawn();
	
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> DeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCHandComponent> HandComponent;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardDrawnOnRoundStart;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardsToDrawThisRound;

	UPROPERTY()
	FOnEndCardDraw OnEndCardDrawDelegate;

	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
	UFUNCTION(BlueprintCallable)
	void DrawCards(int NumberOfCardsToDraw);

	UFUNCTION(BlueprintCallable)
	void DrawCard();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	
};
