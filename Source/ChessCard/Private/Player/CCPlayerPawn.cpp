#include "Player/CCPlayerPawn.h"

#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Deck/CCDeckComponent.h"
#include "Hand/CCHandComponent.h"
ACCPlayerPawn::ACCPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	DeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Deck"));
	HandComponent = CreateDefaultSubobject<UCCHandComponent>(TEXT("Hand"));
}

void ACCPlayerPawn::DrawCards(int NumberOfCardsToDraw)
{
	NumberOfCardsToDrawThisRound = NumberOfCardsToDraw;
	NumberOfCardDrawnOnRoundStart = 0;
	DrawCard();
}

void ACCPlayerPawn::DrawCard()
{
	if(NumberOfCardsToDrawThisRound == NumberOfCardDrawnOnRoundStart)
	{
		return;
	}
		
	HandComponent->GetCards().Add(DeckComponent->CreateCard(FVector()));
	NumberOfCardDrawnOnRoundStart++;

	OnEndCardDrawDelegate.BindDynamic(this, &ACCPlayerPawn::DrawCard);
	int CardNum = HandComponent->GetCardNum();
	HandComponent->GetCards()[CardNum]->CardMovement->StartMovement(CardNum, CardNum, OnEndCardDrawDelegate);
}
