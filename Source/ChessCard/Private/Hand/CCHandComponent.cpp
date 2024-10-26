#include "Hand/CCHandComponent.h"

UCCHandComponent::UCCHandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCCHandComponent::BeginPlay()
{
	Super::BeginPlay();
}