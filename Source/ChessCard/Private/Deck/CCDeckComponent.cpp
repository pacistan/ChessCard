#include "Deck/CCDeckComponent.h"
#include "Card/CCCard.h"
#include "Card/FCardData.h"


UCCDeckComponent::UCCDeckComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ACCCard* UCCDeckComponent::CreateCard(const FVector& SpawnLocation)
{
	const FCardData* CardData = CardDataTable->FindRow<FCardData>(DeckCards[0],TEXT("No Card Found with name"));

	const FActorSpawnParameters SpawnParams;
	auto Card =  GetWorld()->SpawnActor<ACCCard>(CardPrefab, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	Card->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
	return Card;
}

void UCCDeckComponent::BeginPlay()
{
	Super::BeginPlay();
}
