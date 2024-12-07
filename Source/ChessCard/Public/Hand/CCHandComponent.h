#pragma once

#include "CoreMinimal.h"
#include "Card/CCCardMovementComponent.h"
#include "Components/ActorComponent.h"
#include "CCHandComponent.generated.h"


class ACCCard;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHESSCARD_API UCCHandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCHandComponent();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY()
	TArray<TObjectPtr<ACCCard>> Cards;

	UPROPERTY(EditAnywhere)
	float DrawFromDeckAnimDuration;

	UPROPERTY(EditAnywhere)
	float DrawFromMovementDeckAnimDuration;

	UPROPERTY(EditAnywhere)
	float SendCardToMovementDeckAnimDuration;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION()
	int GetCardNum(){return Cards.Num();}

	UFUNCTION()
	void DrawCard(ACCCard* Card, FOnCardMovementEnd OnCardMovementEnd);

	UFUNCTION()
	void SendSelectedCardToMovementDeck(int CardIndex, FOnCardMovementEnd OnCardMovementEnd, FVector DeckPosition);

	UFUNCTION()
	void RemoveCardFromHand(int InCardIndex);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;

	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
};
