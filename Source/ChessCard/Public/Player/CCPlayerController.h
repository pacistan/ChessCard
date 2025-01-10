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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowCardInfo, FDataTableRowHandle, CardRowHandle);


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

	UPROPERTY(VisibleAnywhere, Replicated)
	FString PlayerName;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Default Mapping Context"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Left Click Press Action"))
	UInputAction* IA_SelectCard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerPawn|Input", meta=(DisplayName = "Right Click Press Action"))
	UInputAction* IA_ShowCardInfo;

	TScriptInterface<IHoverable> CurrentHoveredElement;
	
	TScriptInterface<ISelectable> CurrentSelectedElement;

	UPROPERTY(BlueprintAssignable)
	FOnStartGame OnStartGame;

	UPROPERTY(BlueprintAssignable)
	FOnShowCardInfo OnShowCardInfoEvent;

	UPROPERTY(BlueprintAssignable)
	FOnClick OnClickEvent;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:
	UFUNCTION()
	void OnSelectCard();
	
	UFUNCTION()
	void OnShowCardInfo();

	UFUNCTION()
	void OnClick();

	UFUNCTION()
	ACCPlayerPawn* GetCCPlayerPawn();

public:
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	/* ------------------------------------------ GETTERS / SETTERS ------------------------------------*/
public:
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& InPlayerName) { PlayerName = InPlayerName; }
};
