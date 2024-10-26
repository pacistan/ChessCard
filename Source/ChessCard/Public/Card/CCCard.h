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

USTRUCT(BlueprintType)
struct FMaterialArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<UMaterialInterface*> Materials;
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

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> CardMesh;

	UPROPERTY(VisibleAnywhere, Category="")
	ECardState CurrentCardState;

	UPROPERTY()
	int32 CardIndex;

	UPROPERTY(EditDefaultsOnly)
	TMap<ECardState, FMaterialArrayWrapper> MaterialMap;

	UPROPERTY()
	bool IsHovered;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:
	UFUNCTION()
	void UpdateMaterials();

	UFUNCTION()
	void OnSelectCardEffects(bool bIsSelected, ACCPlayerPawn* Pawn);

public:
	UFUNCTION(BlueprintCallable)
	void Play(ACCPlayerPawn* Pawn);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ECardState GetCurrentCardState(){return CurrentCardState;}
	
	/* ------------------------------------------ INTERFACE -------------------------------------------*/
	
	virtual void StartHover(ACCPlayerPawn* Pawn) override;
	
	virtual void StopHover(ACCPlayerPawn* Pawn) override;
	
	virtual void Select(ACCPlayerPawn* Pawn) override;
	
	virtual void UnSelect(ACCPlayerPawn* Pawn) override;

	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintSetter)
	void SetCardIndex(int32 InCardIndex) { CardIndex = InCardIndex;}
};
