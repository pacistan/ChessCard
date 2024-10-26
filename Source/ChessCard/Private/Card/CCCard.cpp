#include "Card/CCCard.h"

#include "Card/CCCardMovementComponent.h"

ACCCard::ACCCard()
{
	PrimaryActorTick.bCanEverTick = true;
	CardMovement = CreateDefaultSubobject<UCCCardMovementComponent>(TEXT("Card Movement"));
}

void ACCCard::StartHover(ACCPlayerPawn* Pawn)
{
}

void ACCCard::StopHover(ACCPlayerPawn* Pawn)
{
}

void ACCCard::Select(ACCPlayerPawn* Pawn)
{
}

void ACCCard::UnSelect(ACCPlayerPawn* Pawn)
{
}

