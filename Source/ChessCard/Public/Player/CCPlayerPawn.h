
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"


class ACCTileUnit;
struct FCardData;
class ACCPieceBase;
class ACCCard;
class UCCMainWidget;
class FOnCardMovementEnd;
class UCCHandComponent;
class UCCDeckComponent;
class UCameraComponent;
class ACCTile;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEndDrawDelegate, ACCPlayerPawn*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedCardChange, int, SelectedCardIndex);

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

	// The player hud class to add to the player when the game start
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PLayerHudClass;

	// Keep a reference to the player hud for removing it when the player is unpossessed
	UPROPERTY()
	TObjectPtr<UUserWidget> PlayerHud;

	UPROPERTY()
	TObjectPtr<ACCTileUnit> SelectedUnit;


public:
	FOnEndDrawDelegate EndDrawDelegate;

	/** Delegate to call when the selected card change, Needed for Ui */
	UPROPERTY(BlueprintAssignable)
	FOnSelectedCardChange OnSelectedCardChange;
	
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

	/** Add the Player HUD to the player, need to be call on Start of the game */
	UFUNCTION(Client, Unreliable)
	void AddPlayerHud();
	void AddPlayerHud_Implementation();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	
	virtual void UnPossessed() override;
	
	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	int32 GetCurrentSelectedCardIndex()const {return CurrentSelectedCardIndex;}
	
	UFUNCTION(BlueprintSetter)
	void SetCurrentSelectedCardIndex(int32 InSelectedCardIndex);

	UFUNCTION(BlueprintGetter)
	UCCHandComponent* GetHandComponent()const {return HandComponent;}

	UFUNCTION(BlueprintGetter)
	TArray<int32> GetPlayedCardIndex()const {return PlayedCardsIndex;}

	UFUNCTION()
	void SetSelectedUnit(ACCTileUnit* Unit);
	
};

