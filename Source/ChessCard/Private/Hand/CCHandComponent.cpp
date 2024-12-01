#include "Hand/CCHandComponent.h"

#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"

UCCHandComponent::UCCHandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCCHandComponent::DrawCard(ACCCard* Card, FOnCardMovementEnd OnCardMovementEnd)
{
	Cards.Add(Card);
	Card->CardMovement->StartMovement( GetCardNum() - 1,  GetCardNum(), OnCardMovementEnd, true, DrawFromDeckAnimDuration);
	if(GetCardNum() == 1) return;
	for(int i = 0; i < GetCardNum() - 1; i++)
	{
		Cards[i]->CardMovement->StartMovement(i, GetCardNum());
	}
}

void UCCHandComponent::BeginPlay()
{
	Super::BeginPlay();
}
