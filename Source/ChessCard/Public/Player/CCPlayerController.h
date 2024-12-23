#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCPlayerController.generated.h"

class ISelectable;
class IHoverable;
class UInputMappingContext;
class UInputAction;
class ACCPlayerPawn;

/**
 * 
 */
UCLASS()
class CHESSCARD_API ACCPlayerController : public APlayerController
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Default Mapping Context"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Left Click Press Action"))
	UInputAction* IA_SelectCard;

	TScriptInterface<IHoverable> CurrentHoveredElement;
	
	TScriptInterface<ISelectable> CurrentSelectedElement;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:
	UFUNCTION()
	void OnSelectCard();

	UFUNCTION()
	ACCPlayerPawn* GetCCPlayerPawn();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;
};
