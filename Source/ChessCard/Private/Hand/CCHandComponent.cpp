#include "Hand/CCHandComponent.h"

#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerController.h"


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

void UCCHandComponent::SendSelectedCardToMovementDeck(int CardIndex, FOnCardMovementEnd OnCardMovementEnd, FVector DeckPosition)
{
	Cards[CardIndex]->CardMovement->StartMovement(
		CardIndex, GetCardNum(), OnCardMovementEnd, true,
		SendCardToMovementDeckAnimDuration, false, true, DeckPosition);
}

FDataTableRowHandle UCCHandComponent::RemoveCardFromHand(int InCardIndex)
{
	ACCCard* Card = Cards[InCardIndex];
	auto PController = Cast<ACCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PController->CurrentSelectedElement.GetObject() == Card)
	{
		PController->CurrentSelectedElement.SetObject(nullptr);
		PController->CurrentSelectedElement.SetInterface(nullptr);
	}
	Cards.RemoveAt(InCardIndex);
	for(int i = 0; i < GetCardNum(); i++)
	{
		Cards[i]->SetCardIndex(i);
		Cards[i]->CardMovement->StartMovement(i, GetCardNum());
	}
	FDataTableRowHandle RowHandle = Card->CardRowHandle;
	Card->Destroy();
	return RowHandle;
}

void UCCHandComponent::BeginPlay()
{
	Super::BeginPlay();
}
