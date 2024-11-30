// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CCActivableWidget.generated.h"

struct FUIInputConfig;

UENUM(BlueprintType)
enum class ECCWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 *  Base class for all activatable widgets in ChessCard
 */
UCLASS(Abstract, Blueprintable)
class CHESSCARD_API UCCActivableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ECCWidgetInputMode InputConfig = ECCWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UCCActivableWidget(const FObjectInitializer& ObjectInitializer);
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface
};
