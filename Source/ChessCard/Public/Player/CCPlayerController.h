#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCPlayerController.generated.h"

class ISelectable;
class IHoverable;
class UInputMappingContext;
class UInputAction;
class ACCPlayerPawn;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGame);

// TODO : Event Show Info Ui Card (CardInfo)

/**
 * 
 */
UCLASS()
class CHESSCARD_API ACCPlayerController : public APlayerController
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InGameUiClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UUserWidget> InGameHud;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Default Mapping Context"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Left Click Press Action"))
	UInputAction* IA_SelectCard;

	TScriptInterface<IHoverable> CurrentHoveredElement;
	
	TScriptInterface<ISelectable> CurrentSelectedElement;

	UPROPERTY(BlueprintAssignable)
	FOnStartGame OnStartGame;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:
	UFUNCTION()
	void OnSelectCard();

	UFUNCTION()
	ACCPlayerPawn* GetCCPlayerPawn();

public:
	UFUNCTION(Client, Reliable)
	void RPC_CreateHudForPlayer();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;
};
