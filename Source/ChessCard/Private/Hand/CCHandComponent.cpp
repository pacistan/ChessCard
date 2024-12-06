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
	Card->CardIndex = GetCardNum() - 1;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, OnCardMovementEnd]()
	{
		Cards[GetCardNum() - 1]->CardMovement->StartMovement( GetCardNum() - 1,  GetCardNum(), OnCardMovementEnd, true, DrawFromDeckAnimDuration, false);
		if(GetCardNum() == 1) return;
		for(int i = 0; i < GetCardNum() - 1; i++)
		{
			Cards[i]->CardMovement->StartMovement(i, GetCardNum());
		}
	});
}

void UCCHandComponent::BeginPlay()
{
	Super::BeginPlay();
}
