#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Selectable.h"
#include "CCCard.generated.h"

class UCCCardMovementComponent;

UENUM()
enum class ECardState : uint8
{
	Inactive,
	Hovered,
	Selected,
	Played,
	IsExamined
};

UCLASS(HideCategories(Rendering, Collision, Actor, Input, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick))
class CHESSCARD_API ACCCard : public AActor, public ISelectable, public IHoverable
{
	GENERATED_BODY()

public:
	ACCCard();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCCardMovementComponent> CardMovement;

	UPROPERTY(VisibleAnywhere, Category="")
	ECardState CurrentCardState;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	/* ------------------------------------------ Getters -------------------------------------------*/
	UFUNCTION(BlueprintGetter)
	ECardState GetCurrentCardState(){return CurrentCardState;}

	/* ------------------------------------------ INTERFACE -------------------------------------------*/
	
	virtual void StartHover(ACCPlayerPawn* Pawn) override;
	
	virtual void StopHover(ACCPlayerPawn* Pawn) override;
	
	virtual void Select(ACCPlayerPawn* Pawn) override;
	
	virtual void UnSelect(ACCPlayerPawn* Pawn) override;
};
