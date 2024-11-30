#include "Player/CCPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interfaces/Hoverable.h"
#include "Interfaces/Selectable.h"
#include "Player/CCPlayerPawn.h"

void ACCPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(PlayerPawn == nullptr) return;

	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	// Do a line trace based on the deprojected location and direction
	FVector TraceEnd = WorldLocation + (WorldDirection * 1000000);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility);
	IHoverable* HoverableActor = Cast<IHoverable>(HitResult.GetActor());

	if (HitResult.bBlockingHit)
	{
		if(CurrentHoveredElement == nullptr) return;
		if(CurrentHoveredElement.GetObject() != HitResult.GetActor())
		{
			if(CurrentHoveredElement.GetObject() != nullptr)
			{
				CurrentHoveredElement->StopHover(PlayerPawn);
			}
			CurrentHoveredElement.SetInterface(HoverableActor);
			CurrentHoveredElement.SetObject(HitResult.GetActor());	
			CurrentHoveredElement->StartHover(PlayerPawn);
		}
	}
	else
	{
		if(CurrentHoveredElement != nullptr)
		{
			CurrentHoveredElement->StopHover(PlayerPawn);
			CurrentHoveredElement = nullptr;
		}
	}
}

void ACCPlayerController::OnSelectCard()
{
	if(PlayerPawn == nullptr) return;

	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

	// Do a line trace based on the deprojected location and direction
	FVector TraceEnd = WorldLocation + (WorldDirection * 1000000);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility);
	ISelectable* SelectableActor = Cast<ISelectable>(HitResult.GetActor());
	
	
	if (HitResult.bBlockingHit)
	{
		if(HitResult.GetActor() != CurrentSelectedElement.GetObject())
		{
			if(CurrentSelectedElement.GetObject() != nullptr)
			{
				CurrentSelectedElement.GetInterface()->UnSelect(PlayerPawn);
			}
			CurrentSelectedElement.SetInterface(SelectableActor);
			CurrentSelectedElement.SetObject(HitResult.GetActor());
			CurrentSelectedElement.GetInterface()->Select(PlayerPawn);
		}
	}
	else
	{
		if(CurrentSelectedElement != nullptr)
		{
			CurrentSelectedElement->UnSelect(PlayerPawn);
			CurrentSelectedElement = nullptr;
		}
	}
}

void ACCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ACCPlayerPawn>(GetPawn());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(IMC_Default, 0);
	}
	bShowMouseCursor = true;
}

void ACCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(IA_SelectCard, ETriggerEvent::Triggered, this, &ACCPlayerController::OnSelectCard);
	}
}


