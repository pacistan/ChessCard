
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"


class ACCCard;
class UCCMainWidget;
class FOnCardMovementEnd;
class UCCHandComponent;
class UCCDeckComponent;
class UCameraComponent;
class ACCTile;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEndDrawDelegate, ACCPlayerPawn*, Player);

UCLASS(/*HideCategories(Rendering, Collision, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick)*/)
class CHESSCARD_API ACCPlayerPawn : public APawn
{
	GENERATED_BODY()

	ACCPlayerPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
private:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> DeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> MovementDeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCHandComponent> HandComponent;

	//UPROPERTY(EditAnywhere, Category = "Camera", meta=(AllowPrivateAccess))
	//UCameraComponent* FollowCamera;
	
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY()
	TObjectPtr<UCCMainWidget> MainWidget;
	
public:
	FOnEndDrawDelegate EndDrawDelegate;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void DrawCards(int NumberOfCardsToDraw);

	UFUNCTION(BlueprintCallable)
	void DrawCard();

	UFUNCTION(BlueprintCallable)
	void PlaySelectedCard(ACCTile* Tiles);

	UFUNCTION(Server, Reliable)
	void OnAllCardDrawServer();

	UFUNCTION(BlueprintCallable)
	void OnGetMovementCardTrigger();

	UFUNCTION(BlueprintCallable)
	void DrawMovementCard();

	UFUNCTION()
	void RemoveCardFromHand();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void PossessedBy(AController* NewController) override;
	
	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	int32 GetCurrentSelectedCardIndex()const {return CurrentSelectedCardIndex;}

	UFUNCTION(BlueprintSetter)
	void SetCurrentSelectedCardIndex(int32 InSelectedCardIndex) ;

	UFUNCTION(BlueprintGetter)
	UCCHandComponent* GetHandComponent()const {return HandComponent;}

	UFUNCTION(BlueprintGetter)
	int32 GetPlayerIndex()const{return PlayerIndex;}

	UFUNCTION(BlueprintSetter)
	void SetPlayerIndex(int32 InPlayerIndex){PlayerIndex = InPlayerIndex;}
	
	UFUNCTION(BlueprintGetter)
	TArray<int32> GetPlayedCardIndex()const {return PlayedCardsIndex;}
};
