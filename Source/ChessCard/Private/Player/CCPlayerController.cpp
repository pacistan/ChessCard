#include "Player/CCPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameModes/CCGameState.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "Interfaces/Selectable.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"

void ACCPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(!GetCCPlayerPawn()) return;

	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	// Do a line trace based on the deprojected location and direction
	FVector TraceEnd = WorldLocation + (WorldDirection * 1000000);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility);
	IHoverable* HoverableActor = Cast<IHoverable>(HitResult.GetActor());

	if (HitResult.bBlockingHit && HoverableActor != nullptr)
	{
		if(CurrentHoveredElement.GetObject() != HitResult.GetActor())
		{
			if(CurrentHoveredElement.GetObject() != nullptr)
			{
				CurrentHoveredElement->StopHover(GetCCPlayerPawn());
			}
			CurrentHoveredElement.SetInterface(HoverableActor);
			CurrentHoveredElement.SetObject(HitResult.GetActor());	
			CurrentHoveredElement->StartHover(GetCCPlayerPawn());
		}
	}
	else
	{
		if(CurrentHoveredElement != nullptr)
		{
			CurrentHoveredElement->StopHover(GetCCPlayerPawn());
			CurrentHoveredElement = nullptr;
		}
	}
}

void ACCPlayerController::OnSelectCard()
{
	ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>();
	check(GameState);
	if(!GetCCPlayerPawn()) return;
	if(GameState->GetCurrentState() != EGameState::Plannification)
	{
		if(CurrentSelectedElement != nullptr)
		{
			CurrentSelectedElement->UnSelect(GetCCPlayerPawn());
			CurrentSelectedElement = nullptr;
		}
		GetCCPlayerPawn()->SetSelectedUnit(nullptr);
		return;
	}
	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	// Do a line trace based on the deprojected location and direction
	FVector TraceEnd = WorldLocation + (WorldDirection * 1000000);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility);
	ISelectable* SelectableActor = Cast<ISelectable>(HitResult.GetActor());
	IClickable* ClickableActor = Cast<IClickable>(HitResult.GetActor());

	if(ClickableActor != nullptr)
	{
		ClickableActor->Click(GetCCPlayerPawn());
	}
	
	if (HitResult.bBlockingHit && SelectableActor != nullptr)
	{
		if(HitResult.GetActor() != CurrentSelectedElement.GetObject())
		{
			if(CurrentSelectedElement.GetObject() != nullptr)
			{
				CurrentSelectedElement.GetInterface()->UnSelect(GetCCPlayerPawn());
			}
			CurrentSelectedElement.SetInterface(SelectableActor);
			CurrentSelectedElement.SetObject(HitResult.GetActor());
			CurrentSelectedElement.GetInterface()->Select(GetCCPlayerPawn());
		}
	}
	else
	{
		if(CurrentSelectedElement != nullptr)
		{
			//CurrentSelectedElement->UnSelect(GetCCPlayerPawn());
			//CurrentSelectedElement = nullptr;
		}
	}
}

void ACCPlayerController::OnShowCardInfo()
{
	ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>();
	check(GameState);
	if(!GetCCPlayerPawn()|| GameState->GetCurrentState() == EGameState::None || GameState->GetCurrentState() == EGameState::EndGame){
		return;
	}
	
	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	// Do a line trace based on the deprojected location and direction
	FVector TraceEnd = WorldLocation + (WorldDirection * 1000000);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility);
	IHoverable* HoverableActor = Cast<IHoverable>(HitResult.GetActor());

	// If Find Actor brodcast the event with the card data
	if (HoverableActor != nullptr) {
		FDataTableRowHandle CardRowHandle;
		if (HoverableActor->GetCardData(CardRowHandle)) {
			OnShowCardInfoEvent.Broadcast(CardRowHandle);
			// TODO Activate the event for Hide the card info
		}
    } 
	
}

void ACCPlayerController::OnClick()
{
	OnClickEvent.Broadcast();
}

ACCPlayerPawn* ACCPlayerController::GetCCPlayerPawn()
{
	return Cast<ACCPlayerPawn>(GetPawn());
}

void ACCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(IMC_Default, 0);
	}
	bShowMouseCursor = true;
}

void ACCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(IA_SelectCard, ETriggerEvent::Triggered, this, &ACCPlayerController::OnSelectCard);
		EnhancedInputComponent->BindAction(IA_ShowCardInfo, ETriggerEvent::Triggered, this, &ACCPlayerController::OnShowCardInfo);
		EnhancedInputComponent->BindAction(IA_ShowCardInfo, ETriggerEvent::Triggered, this, &ACCPlayerController::OnClick);
		EnhancedInputComponent->BindAction(IA_SelectCard, ETriggerEvent::Triggered, this, &ACCPlayerController::OnClick);
	}
}


