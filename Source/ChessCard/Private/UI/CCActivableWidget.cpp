// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CCActivableWidget.h"

UCCActivableWidget::UCCActivableWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UCCActivableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ECCWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ECCWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ECCWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case ECCWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
