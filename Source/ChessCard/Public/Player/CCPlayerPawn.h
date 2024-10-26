
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"
class UCCHandComponent;
class UCCDeckComponent;
class ACCTile;

UCLASS(HideCategories(Rendering, Collision, Actor, Input, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick))
class CHESSCARD_API ACCPlayerPawn : public APawn
{
	GENERATED_BODY()

	ACCPlayerPawn();
	
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
private:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> DeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCHandComponent> HandComponent;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardDrawnOnRoundStart;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardsToDrawThisRound;

	UPROPERTY()
	TArray<int32> PlayedCardsIndex;

	// -1 = No card Selected
	UPROPERTY()
	int32 CurrentSelectedCardIndex;

	UPROPERTY()
	int32 PlayerIndex;

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION(BlueprintCallable)
	void DrawCards(int NumberOfCardsToDraw);

	UFUNCTION(BlueprintCallable)
	void DrawCard();

	UFUNCTION(BlueprintCallable)
	void PlaySelectedCard(ACCTile* Tiles);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	int32 GetCurrentSelectedCardIndex()const {return CurrentSelectedCardIndex;}

	UFUNCTION(BlueprintSetter)
	void SetCurrentSelectedCardIndex(int32 InSelectedCardIndex) {CurrentSelectedCardIndex = InSelectedCardIndex;};

	UFUNCTION(BlueprintGetter)
	UCCHandComponent* GetHandComponent()const {return HandComponent;}

	UFUNCTION(BlueprintGetter)
	int32 GetPlayerIndex()const{return PlayerIndex;}

	UFUNCTION(BlueprintSetter)
	void SetPlayerIndex(int32 InPlayerIndex){PlayerIndex = InPlayerIndex;}
	
	UFUNCTION(BlueprintGetter)
	TArray<int32> GetPlayedCardIndex()const {return PlayedCardsIndex;}
};
